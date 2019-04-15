#include "unresolved_block_pool.hpp"

#include <time.h>

namespace gruut {

UnresolvedBlockPool::UnresolvedBlockPool() {
  m_storage_manager = StorageManager::getInstance();
  el::Loggers::getLogger("URBK");
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
    if (block.getPrevBlockId() == m_latest_confirmed_prev_id) {
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

  block_height_type aa = block.getHeight();

  invalidateCaches(); // 캐시 관련 함수. 추후 고려.

  m_push_mutex.unlock();

  return ret_val;
}

void UnresolvedBlockPool::process_tx_result(Block &new_block, nlohmann::json &result) {
  // 새로운 블럭에 대한 정보 검증을 완료한다(state root 값 검증 등)
  // 파싱한 결과로 unresolved_block에 저장될 ledger에 대한 정보를 만들고 처리

  if (new_block.getBlockId() != m_head_id) {
    move_head(new_block.getPrevBlockId(), new_block.getHeight());
  }

  base64_type block_id = json::get<string>(result["block"], "id").value();
  base64_type block_height = json::get<string>(result["block"], "height").value();

  // process result json, make ledger etc..

  // state root 구성한 후 비교
  calcStateRoot(); // usroot
  calcStateRoot(); // csroot
}

void UnresolvedBlockPool::move_head(const base64_type &target_block_id_b64, const block_height_type target_block_height) {
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
      CLOG(ERROR, "URBK") << "Something error in move_head() - Cannot find pool element";
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
        CLOG(ERROR, "URBK") << "Something error in move_head() - Find common ancestor(1)";
        return; // 나중에 예외처리 한 번 더 살필 것
      }
    }

    if (target_bin_idx == -1 && current_bin_idx == -1) {
      if (target_queue_idx != 0 || current_queue_idx != 0) {
        CLOG(ERROR, "URBK") << "Something error in move_head() - Find common ancestor(2)";
        return;
      }
    } else if (m_block_pool[target_bin_idx][target_queue_idx].block.getBlockId() !=
               m_block_pool[current_bin_idx][current_queue_idx].block.getBlockId()) {
      CLOG(ERROR, "URBK") << "Something error in move_head() - Find common ancestor(2)";
      return;
    }

    // 만남지점까지 back, 그리고 new_head까지 forward하는 함수를 구현
    // 이거 구현하려면 ledger 관련, state tree 관련부터 해야함. 그래야 할 수 있음.

    for (auto &deque_aa :)
      where_to_go[0].first, where_to_go[0].second; // front를 하기 위한 방향 저장된것.
  }
}

void UnresolvedBlockPool::getResolvedBlocks(std::vector<UnresolvedBlock> &resolved_blocks, std::vector<string> &drop_blocks) {
  resolved_blocks.clear();
  drop_blocks.clear();

  auto storage = Storage::getInstance();
  size_t num_resolved_block = 0;
  nlohmann::json del_id_array = nlohmann::json::array();
  nlohmann::json new_id_array = nlohmann::json::array();

  auto resolveBlocksStepByStep = [this](std::vector<UnresolvedBlock> &resolved_blocks, std::vector<string> &drop_blocks) {
    updateConfirmLevel();

    if (m_block_pool.size() < 2 || m_block_pool[0].empty() || m_block_pool[1].empty())
      return;

    size_t highest_confirm_level = 0;
    int resolved_block_idx = -1;

    for (size_t i = 0; i < m_block_pool[0].size(); ++i) {
      if (m_block_pool[0][i].prev_vector_idx == 0 && m_block_pool[0][i].confirm_level > highest_confirm_level) {
        highest_confirm_level = m_block_pool[0][i].confirm_level;
        resolved_block_idx = static_cast<int>(i);
      }
    }

    if (resolved_block_idx < 0 || highest_confirm_level < config::BLOCK_CONFIRM_LEVEL)
      return; // nothing to do

    bool is_after = false;
    for (auto &each_block : m_block_pool[1]) {
      if (each_block.prev_vector_idx == resolved_block_idx) { // some block links this block
        is_after = true;
        break;
      }
    }

    if (!is_after)
      return;

    m_prev_block_id = m_last_block_id;

    m_latest_confirmed_id = m_block_pool[0][resolved_block_idx].block.getBlockId();
    m_latest_confirmed_hash = m_block_pool[0][resolved_block_idx].block.getBlockHash();
    m_latest_confirmed_height = m_block_pool[0][resolved_block_idx].block.getHeight();

    resolved_blocks.emplace_back(m_block_pool[0][resolved_block_idx]);

    // clear this height list

    auto layered_storage = LayeredStorage::getInstance();

    if (m_block_pool[0].size() > 1) {
      for (auto &each_block : m_block_pool[0]) {
        if (each_block.block.getBlockId() != m_latest_confirmed_id) {
          drop_blocks.emplace_back(each_block.block.getBlockId());
        }
      }
      CLOG(INFO, "URBK") << "Dropped out " << m_block_pool[0].size() - 1 << " unresolved block(s)";
    }

    m_block_pool.pop_front();

    if (m_block_pool.empty())
      return;

    for (auto &each_block : m_block_pool[0]) {
      if (each_block.block.getPrevBlockId() == m_last_block_id && each_block.block.getPrevHash() == m_last_hash) {
        each_block.prev_vector_idx = 0;
      } else {
        // this block is unlinkable => to be deleted
        each_block.prev_vector_idx = -1;
      }
    }
  };

  std::lock_guard<std::recursive_mutex> guard(m_push_mutex);

  do {
    num_resolved_block = resolved_blocks.size();
    resolveBlocksStepByStep(resolved_blocks, drop_blocks);
  } while (num_resolved_block < resolved_blocks.size());

  json id_array = readBackupIds();

  if (id_array.empty() || !id_array.is_array())
    return;

  for (auto &each_block_id : drop_blocks) {
    del_id_array.push_back(each_block_id);
  }

  for (auto &each_id : id_array) {
    bool is_resolved = false;
    string block_id_b64 = json::get<string>(each_id).value();
    if (block_id_b64.empty())
      continue;

    for (auto &each_block : resolved_blocks) {
      if (block_id_b64 == each_block.block.getBlockIdB64()) {
        is_resolved = true;
        break;
      }
    }

    if (is_resolved) {
      del_id_array.push_back(block_id_b64);
    } else {
      new_id_array.push_back(block_id_b64);
    }
  }

  storage->saveBackup(UNRESOLVED_BLOCK_IDS_KEY, TypeConverter::bytesToString(json::to_cbor(new_id_array)));

  for (auto &each_id : del_id_array) {
    string block_id_b64 = json::get<string>(each_id).value();
    storage->delBackup(block_id_b64);
  }

  storage->flushBackup();

  m_push_mutex.unlock();
}

void UnresolvedBlockPool::updateConfirmLevel() {
  if (m_block_pool.empty())
    return;

  for (auto &each_level : m_block_pool) {
    for (auto &each_block : each_level) {
      each_block.confirm_level = each_block.block.getNumSSigs();
    }
  }

  for (int i = (int)m_block_pool.size() - 1; i > 0; --i) {
    for (auto &each_block : m_block_pool[i]) { // for vector
      if (each_block.prev_vector_idx >= 0 && m_block_pool[i - 1].size() > each_block.prev_vector_idx) {
        m_block_pool[i - 1][each_block.prev_vector_idx].confirm_level += each_block.confirm_level;
      }
    }
  }
}

BlockPosPool UnresolvedBlockPool::getLongestBlockPos() {
  //  if (m_has_cache_pos)
  //    return m_cache_possible_pos;

  // search prev_level
  std::function<void(size_t, size_t, BlockPosPool &)> recSearchLongestLinkPos;
  recSearchLongestLinkPos = [this, &recSearchLongestLinkPos](size_t bin_idx, size_t prev_vector_idx, BlockPosPool &longest_pos) {
    if (m_block_pool.size() < bin_idx + 1)
      return;

    // if this level is empty, it will automatically stop to search
    for (size_t i = 0; i < m_block_pool[bin_idx].size(); ++i) {
      auto &each_block = m_block_pool[bin_idx][i];
      if (each_block.prev_vector_idx == prev_vector_idx) {
        if (each_block.block.getHeight() > longest_pos.height) { // relaxation
          longest_pos.height = each_block.block.getHeight();
          longest_pos.vector_idx = i;
        }

        recSearchLongestLinkPos(bin_idx + 1, i, longest_pos);
      }
    }
  };

  BlockPosPool longest_pos(0, 0);

  if (!m_block_pool.empty()) {
    for (size_t i = 0; i < m_block_pool[0].size(); ++i) {
      if (m_block_pool[0][i].prev_vector_idx == 0) {

        if (m_block_pool[0][i].block.getHeight() > longest_pos.height) { // relaxation
          longest_pos.height = m_block_pool[0][i].block.getHeight();
          longest_pos.vector_idx = i;
        }

        recSearchLongestLinkPos(1, i, longest_pos); // recursive call
      }
    }
  }

  //  m_cache_possible_pos = longest_pos;
  //  m_has_cache_pos = true;

  return longest_pos;
}

nth_link_type UnresolvedBlockPool::getMostPossibleLink() {

  //  if (m_has_cache_link)
  //    return m_cache_possible_link;

  nth_link_type ret_link;

  std::lock_guard<std::recursive_mutex> guard(m_push_mutex);

  // when resolved situation, the blow are the same with storage
  // unless, they are faster than storage

  ret_link.height = m_latest_confirmed_height;
  ret_link.id = m_latest_confirmed_id;
  ret_link.prev_id = m_latest_confirmed_prev_id;
  ret_link.time = m_latest_confirmed_time;

  if (m_block_pool.empty()) {
    //    m_cache_possible_link = ret_link;
    //    m_has_cache_link = true;
    return ret_link;
  }

  BlockPosPool longest_pos = getLongestBlockPos();

  if (ret_link.height < longest_pos.height) {

    // must be larger or equal to 0
    int deque_idx = static_cast<int>(longest_pos.height - m_last_height) - 1;

    if (deque_idx >= 0) {
      auto &t_block = m_block_pool[deque_idx][longest_pos.vector_idx];
      ret_link.height = t_block.block.getHeight();
      ret_link.id = t_block.block.getBlockId();
      ret_link.hash = t_block.block.getHash();
      ret_link.prev_id = t_block.block.getPrevBlockId();
      ret_link.time = t_block.block.getBlockTime();
    }
  }

  m_cache_possible_link = ret_link;
  m_has_cache_link = true;

  return ret_link;
}

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
json UnresolvedBlockPool::readBackupIds() {}
nth_link_type UnresolvedBlockPool::getUnresolvedLowestLink() {}

} // namespace gruut
