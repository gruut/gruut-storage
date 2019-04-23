#include "unresolved_block_pool.hpp"

#include <time.h>

namespace gruut {

UnresolvedBlockPool::UnresolvedBlockPool() {

  el::Loggers::getLogger("URBP");
}

void UnresolvedBlockPool::setPool(const base64_type &last_block_id, block_height_type last_height, timestamp_t last_time,
                                  const base64_type &last_hash, const base64_type &prev_block_id) {
  m_latest_confirmed_id = last_block_id;
  m_latest_confirmed_height = last_height;
  m_latest_confirmed_time = last_time;
  m_latest_confirmed_hash = last_hash;
  m_latest_confirmed_prev_id = prev_block_id;
}

// 블록이 push될 때마다 실행되는 함수. pool이 되는 벡터를 resize 하는 등의 컨트롤을 한다.
bool UnresolvedBlockPool::prepareBins(block_height_type t_height) {
  std::lock_guard<std::recursive_mutex> guard(m_push_mutex);
  if (m_latest_confirmed_height >= t_height) {
    return false;
  }

  if ((TimeUtil::nowBigInt() - m_latest_confirmed_time) < (t_height - m_latest_confirmed_height - 1) * config::BP_INTERVAL) {
    return false;
  }

  int bin_pos = static_cast<int>(t_height - m_latest_confirmed_height) - 1; // deque에서 위치하는 인덱스. e.g., 0 = 2 - 1 - 1
  if (m_block_pool.size() < bin_pos + 1) {
    m_block_pool.resize(bin_pos + 1);
  }

  return true;
}

unblk_push_result_type UnresolvedBlockPool::push(Block &block, bool is_restore) {
  unblk_push_result_type ret_val; // 해당 return 구조는 추후 변경 가능성 있음
  ret_val.height = 0;
  ret_val.linked = false;
  ret_val.duplicated = false;

  std::lock_guard<std::recursive_mutex> guard(m_push_mutex);

  block_height_type block_height = block.getHeight();
  int bin_idx = static_cast<int>(block_height - m_latest_confirmed_height) - 1; // e.g., 0 = 2 - 1 - 1
  if (!prepareBins(block_height))
    return ret_val;

  for (auto &each_block : m_block_pool[bin_idx]) {
    if (each_block.block == block) {
      ret_val.height = block_height;
      ret_val.duplicated = true;
      return ret_val;
    }
  }

  int prev_queue_idx = -1; // no previous

  if (bin_idx > 0) { // if there is previous bin
    size_t idx = 0;
    for (auto &each_block : m_block_pool[bin_idx - 1]) {
      if (each_block.block.getBlockId() == block.getPrevBlockId()) {
        prev_queue_idx = static_cast<int>(idx);
        break;
      }
      ++idx;
    }
  } else { // no previous
    if (block.getPrevBlockId() == m_latest_confirmed_id) {
      prev_queue_idx = 0;
    } else {
      // drop block -- this is not linkable block!
      return ret_val;
    }
  }

  int queue_idx = m_block_pool[bin_idx].size();

  m_block_pool[bin_idx].emplace_back(block, prev_queue_idx); // pool에 블록 추가

  ret_val.height = block_height;

  if (!is_restore)
    backupPool();

  if (bin_idx + 1 < m_block_pool.size()) { // if there is next bin
    for (auto &each_block : m_block_pool[bin_idx + 1]) {
      if (each_block.block.getPrevBlockId() == block.getBlockId()) {
        if (each_block.prev_vector_idx < 0) {
          each_block.prev_vector_idx = queue_idx;
        }
      }
    }
  }

  invalidateCaches(); // 캐시 관련 함수. 추후 고려.

  m_push_mutex.unlock();

  return ret_val;
}

void UnresolvedBlockPool::processTxResult(UnresolvedBlock &new_UR_block, nlohmann::json &result) {
  // 파싱한 결과로 unresolved_block에 저장될 ledger에 대한 정보를 만들고 처리

  if (new_UR_block.block.getBlockId() != m_head_id) {
    moveHead(new_UR_block.block.getPrevBlockId(), new_UR_block.block.getHeight());
  }

  // process result json, make ledger etc..
  base64_type block_id = json::get<string>(result["block"], "id").value();
  base64_type block_height = json::get<string>(result["block"], "height").value();

  nlohmann::json results_json = result["results"];
  for (auto &each_result : results_json) {
    base58_type tx_id = json::get<string>(each_result, "txid").value();
    bool status = json::get<bool>(each_result, "status").value();
    string info = json::get<string>(each_result, "info").value();
    base58_type author = json::get<string>(each_result["authority"], "author").value();
    base58_type user = json::get<string>(each_result["authority"], "user").value();
    base58_type receiver = json::get<string>(each_result["authority"], "receiver").value();
    string self = json::get<string>(each_result["authority"], "self").value();

    vector<string> friends;
    nlohmann::json friends_json = each_result["friend"];
    for (auto &each_friend : friends_json) {
      friends.emplace_back(each_friend.dump());
    }
    int fee = stoi(json::get<string>(each_result, "fee").value());

    nlohmann::json queries_json = each_result["queries"];
    for (auto &each_query : queries_json) {
      string type = json::get<string>(each_query, "type").value();
      nlohmann::json option = each_query["option"];
      if (type == "user.join") {
        queryUserJoin(new_UR_block, option);
      } else if (type == "user.cert") {
        queryUserCert(new_UR_block, option);
      } else if (type == "v.incinerate") {
        queryIncinerate(new_UR_block, option);
      } else if (type == "v.create") {
        queryCreate(new_UR_block, option);
      } else if (type == "v.transfer") {
        queryTransfer(new_UR_block, option);
      } else if (type == "scope.user") {
        queryUserScope(new_UR_block, option);
      } else if (type == "scope.contract") {
        queryContractScope(new_UR_block, option);
      } else if (type == "run.query") {
        queryRunQuery(new_UR_block, option);
      } else if (type == "run.contract") {
        queryRunContract(new_UR_block, option);
      } else {
        CLOG(ERROR, "URBP") << "Something error in query process";
      }
    }
  }

  //  calcStateRoot(usroot);  // TODO: 추후 구현
  //  calcStateRoot(csroot);  // TODO: 추후 구현
}

void UnresolvedBlockPool::moveHead(const base64_type &target_block_id_b64, const block_height_type target_block_height) {
  // missing link 등에 대한 예외처리를 추가해야 할 필요 있음

  if (!target_block_id_b64.empty()) {
    // latest_confirmed의 height가 10이었고, 현재 옮기려는 타겟의 height가 12라면 m_block_pool[1]에 있어야 한다
    int target_height = target_block_height;
    int target_bin_idx = static_cast<int>(target_block_height - m_latest_confirmed_height) - 1;
    int target_queue_idx = -1;

    // new_head를 pool에서 어디에 있는지 찾음
    for (size_t i = 0; i < m_block_pool[target_bin_idx].size(); ++i) {
      if (target_block_id_b64 == m_block_pool[target_bin_idx][i].block.getBlockId()) {
        target_queue_idx = static_cast<int>(i);
        break;
      }
    }

    // 현재 head부터 confirmed까지의 경로 구함 -> vector를 resize해서 depth에 맞춰서 집어넣음
    // latest_confirmed의 height가 10이었고, 현재 head의 height가 11이라면 m_block_pool[0]에 있어야 한다
    int current_height = static_cast<int>(m_head_height);
    int current_bin_idx = static_cast<int>(m_head_height - m_latest_confirmed_height) - 1;
    int current_queue_idx = -1;

    for (size_t i = 0; i < m_block_pool[current_bin_idx].size(); ++i) {
      if (m_head_id == m_block_pool[current_bin_idx][i].block.getBlockId()) {
        current_queue_idx = static_cast<int>(i);
        break;
      }
    }

    if (target_queue_idx == -1 || current_queue_idx == -1) {
      CLOG(ERROR, "URBP") << "Something error in move_head() - Cannot find pool element";
      return;
    }

    // height가 높은 쪽을 아래로 가지고 내려오고, height가 같아지면 서로 하나씩 내려가며 비교
    int back_count = 0;
    int front_count = 0;
    std::deque<pair<int, int>> where_to_go;
    where_to_go.clear();

    while (target_height != current_height) {
      if (target_height > current_height) {
        where_to_go.emplace_front(target_bin_idx, target_queue_idx);
        target_queue_idx = m_block_pool[target_bin_idx][target_queue_idx].prev_vector_idx;
        target_height--;
        target_bin_idx--;

        front_count++;
      } else if (target_height < current_height) {
        current_queue_idx = m_block_pool[current_bin_idx][current_queue_idx].prev_vector_idx;
        current_height--;
        current_bin_idx--;

        back_count++;
      }
    }

    while ((target_bin_idx != current_bin_idx) || (target_queue_idx != current_queue_idx)) {
      where_to_go.emplace_front(target_bin_idx, target_queue_idx);
      target_queue_idx = m_block_pool[target_bin_idx][target_queue_idx].prev_vector_idx;
      target_height--;
      target_bin_idx--;
      front_count++;

      current_queue_idx = m_block_pool[current_bin_idx][current_queue_idx].prev_vector_idx;
      current_height--;
      current_bin_idx--;
      back_count++;

      if (target_bin_idx < -1 || current_bin_idx < -1) {
        CLOG(ERROR, "URBP") << "Something error in move_head() - Find common ancestor(1)";
        return; // 나중에 예외처리 한 번 더 살필 것
      }
    }

    if (target_bin_idx == -1 && current_bin_idx == -1) {
      if (target_queue_idx != 0 || current_queue_idx != 0) {
        CLOG(ERROR, "URBP") << "Something error in move_head() - Find common ancestor(2)";
        return;
      }
    } else if (m_block_pool[target_bin_idx][target_queue_idx].block.getBlockId() !=
               m_block_pool[current_bin_idx][current_queue_idx].block.getBlockId()) {
      CLOG(ERROR, "URBP") << "Something error in move_head() - Find common ancestor(2)";
      return;
    }

    // 만남지점까지 back, 그리고 new_head까지 forward하는 함수를 구현
    // 이거 구현하려면 ledger 관련, state tree 관련부터 해야함. 그래야 할 수 있음.

    for (auto &deque_aa :)
      where_to_go[0].first, where_to_go[0].second; // front를 하기 위한 방향 저장된것.
  }
}

bool UnresolvedBlockPool::queryUserJoin(UnresolvedBlock &UR_block, nlohmann::json &option) {
  base58_type uid = json::get<string>(option, "uid").value();
  string gender = json::get<string>(option, "gender").value();
  int age = stoi(json::get<string>(option, "age").value());
  string isc_type = json::get<string>(option, "isc_type").value();
  string isc_code = json::get<string>(option, "isc_code").value();
  string location = json::get<string>(option, "location").value();

  // TODO: 2.1.2. User Attributes 테이블에 추가하는 코드 작성
}

bool UnresolvedBlockPool::queryUserCert(UnresolvedBlock &UR_block, nlohmann::json &option) {
  base58_type uid = json::get<string>(option, "uid").value();
  timestamp_t notbefore = static_cast<uint64_t>(stoll(json::get<string>(option, "notbefore").value()));
  timestamp_t notafter = static_cast<uint64_t>(stoll(json::get<string>(option, "notafter").value()));
  string sn = json::get<string>(option, "sn").value();
  string x509 = json::get<string>(option, "x509").value();

  // TODO: 2.1.1. User Certificates 테이블에 추가하는 코드 작성
}

bool UnresolvedBlockPool::queryIncinerate(UnresolvedBlock &UR_block, nlohmann::json &option) {
  int amount = stoi(json::get<string>(option, "amount").value());
  string pid = json::get<string>(option, "pid").value();

  // TODO: m_mem_ledger 사용하여 갱신값 계산
}

bool UnresolvedBlockPool::queryCreate(UnresolvedBlock &UR_block, nlohmann::json &option) {
  int amount = stoi(json::get<string>(option, "amount").value());
  string name = json::get<string>(option, "name").value();
  string type = json::get<string>(option, "type").value();
  string condition = json::get<string>(option, "condition").value();

  // TODO: m_mem_ledger 사용하여 갱신값 계산
}

bool UnresolvedBlockPool::queryTransfer(UnresolvedBlock &UR_block, nlohmann::json &option) {
  base58_type from = json::get<string>(option, "from").value();
  base58_type to = json::get<string>(option, "to").value();
  int amount = stoi(json::get<string>(option, "amount").value());
  string unit = json::get<string>(option, "unit").value();
  string pid = json::get<string>(option, "pid").value();
  string condition = json::get<string>(option, "condition").value();

  // TODO: m_mem_ledger 사용하여 갱신값 계산
  // from과 to가 user/contract 따라 처리될 수 있도록 구현 필요
}

bool UnresolvedBlockPool::queryUserScope(UnresolvedBlock &UR_block, nlohmann::json &option) {
  string name = json::get<string>(option, "name").value();
  string value = json::get<string>(option, "value").value();
  base58_type uid = json::get<string>(option, "uid").value();
  string pid = json::get<string>(option, "pid").value();
  string condition = json::get<string>(option, "condition").value();

  // TODO: m_mem_ledger 사용하여 갱신값 계산
}

bool UnresolvedBlockPool::queryContractScope(UnresolvedBlock &UR_block, nlohmann::json &option) {
  string name = json::get<string>(option, "name").value();
  string value = json::get<string>(option, "value").value();
  string cid = json::get<string>(option, "cid").value();
  string pid = json::get<string>(option, "pid").value();

  // TODO: m_mem_ledger 사용하여 갱신값 계산
}

bool UnresolvedBlockPool::queryRunQuery(UnresolvedBlock &UR_block, nlohmann::json &option) {
  string type = json::get<string>(option, "type").value();
  nlohmann::json query = option["query"];
  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));

  // TODO: Scheduler에게 지연 처리 요청 전송
}

bool UnresolvedBlockPool::queryRunContract(UnresolvedBlock &UR_block, nlohmann::json &option) {
  string cid = json::get<string>(option, "cid").value();
  string input = json::get<string>(option, "input").value();
  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));

  // TODO: authority.user를 현재 user로 대체하여 Scheduler에게 요청 전송
}

void UnresolvedBlockPool::getResolvedBlocks(std::vector<UnresolvedBlock> &resolved_blocks, std::vector<string> &drop_blocks) {}

void UnresolvedBlockPool::updateTotalNumSSig() {
  if (m_block_pool.empty())
    return;

  for (auto &each_level : m_block_pool) {
    for (auto &each_block : each_level) {
      each_block.ssig_sum = each_block.block.getNumSigners();
    }
  }

  for (int i = (int)m_block_pool.size() - 1; i > 0; --i) {
    for (auto &each_block : m_block_pool[i]) { // for vector
      if (each_block.prev_vector_idx >= 0 && m_block_pool[i - 1].size() > each_block.prev_vector_idx) {
        m_block_pool[i - 1][each_block.prev_vector_idx].ssig_sum += each_block.ssig_sum;
      }
    }
  }
}

BlockPosPool UnresolvedBlockPool::getLongestBlockPos() {}

void UnresolvedBlockPool::setupStateTree() // RDB에 있는 모든 노드를 불러올 수 있어야 하므로 관련 연동 필요
{
  vector<pair<int, vector<string>>> all = m_server.selectAll();
  for (auto item : all) {
    //                printf("%5d\t", item.first);
    //                for(auto column: item.second)
    //                    printf("%15s\t", column.c_str());
    //                printf("\n");

    test_data data;
    //  data.record_id = item.first;
    data.user_id = item.second[USER_ID];
    data.var_type = item.second[VAR_TYPE];
    data.var_name = item.second[VAR_NAME];
    data.var_value = item.second[VAR_VALUE];
    uint path = (uint)stoul(item.second[PATH]);
    m_user_state_tree.addNode(path, data);
  }

  m_user_state_tree.printTreePostOrder();
}

// 추후 필요시 구현
bool UnresolvedBlockPool::hasUnresolvedBlocks() {}
void UnresolvedBlockPool::invalidateCaches() {}
bool UnresolvedBlockPool::getBlock(block_height_type t_height, const hash_t &t_prev_hash, const hash_t &t_hash, Block &ret_block) {}
bool UnresolvedBlockPool::getBlock(block_height_type t_height, Block &ret_block) {}
nlohmann::json UnresolvedBlockPool::readBackupIds() {}
nth_link_type UnresolvedBlockPool::getUnresolvedLowestLink() {}

} // namespace gruut
