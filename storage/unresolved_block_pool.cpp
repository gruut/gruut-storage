//
// Created by msk on 2019-03-25.
//

#include "unresolved_block_pool.hpp"
#include <time.h>
namespace gruut {

UnresolvedBlockPool::UnresolvedBlockPool() {
  m_storage_manager = StorageManager::getInstance();
  el::Loggers::getLogger("URBK");
}

void UnresolvedBlockPool::setPool(const block_id_type &last_block_id, const block_id_type &prev_block_id, const hash_t &prev_hash,
                                  block_height_type last_height, timestamp_t last_time) {
  m_latest_confirmed_block_id = last_block_id;
  m_latest_confirmed_prev_block_id = prev_block_id;
  m_latest_confirmed_block_height = last_height;
  m_latest_confirmed_block_time = last_time;
  m_height_range_max = last_height;
}

bool UnresolvedBlockPool::prepareBins(block_height_type t_height) {
  std::lock_guard<std::recursive_mutex> guard(m_push_mutex);
  if (m_latest_confirmed_block_height >= t_height) {
    return false;
  }

  if ((Time::now_int() - m_latest_confirmed_block_time) < (t_height - m_latest_confirmed_block_height - 1) * config::BP_INTERVAL) {
    return false;
  }

  int bin_pos = static_cast<int>(t_height - m_latest_confirmed_block_height) - 1; // e.g., 0 = 2 - 1 - 1

  if (m_block_pool.size() < bin_pos + 1) {
    m_block_pool.resize(bin_pos + 1);
  }

  return true;
}

unblk_push_result_type UnresolvedBlockPool::push(Block &block, bool is_restore) {
  // 일단 block 정보를 저장, 각종 초기화 함수들 사용
  // enterprise때와 같이 m_block_pool에 넣는 작업 시행
  // pool에 해당 블록을 넣기만 하고 다른 작업은 하지 않는다.
}

void UnresolvedBlockPool::process_tx_result(const Block &new_block, json result) {
  if (new_block.m_latest_confirmed_prev_block_id = !m_head_block_id) {
    move_head(new_block.m_latest_confirmed_prev_block_id, ); // head를 옮겨야 할 경우
  }

  // 새로운 블럭에 대한 정보 입력을 완료한다(state root 값 입력 등)
  // result의 json을 받아서 파싱하는것도 여기에서 해야한다.
  // 그 결과로 임시 ledger에 대한 정보도 여기에서 입력해야 함
  // resolved 검사를 언제 할지는 여기서 할지 push에서 할지 조금 더 생각해볼 필요 있음
}

void UnresolvedBlockPool::move_head(const std::string &target_block_id_b64, const block_height_type target_block_height) {
  // b64인지 기본 해시형인지 수정할 필요 있음, missing link 등에 대한 예외처리도 필요할 듯 함

  if (!target_block_id_b64.empty()) {
    // latest_confirmed의 height가 10이었고, 현재 옮기려는 타겟의 height가 12라면 m_block_pool[1]에 있어야 한다
    int target_height = target_block_height;
    int target_bin_idx = static_cast<int>(target_block_height - m_latest_confirmed_block_height) - 1;
    int target_queue_idx = -1;

    // new_head를 pool에서 어디에 있는지 찾음
    for (size_t i = 0; i < m_block_pool[target_bin_idx].size(); ++i) {
      if (target_block_id_b64 == m_block_pool[target_bin_idx][i].block.getBlockIdB64()) {
        target_queue_idx = static_cast<int>(i);
        break;
      }
    }

    // 현재 head부터 confirmed까지의 경로 구함 -> vector를 resize해서 depth에 맞춰서 집어넣음
    // latest_confirmed의 height가 10이었고, 현재 head의 height가 11이라면 m_block_pool[0]에 있어야 한다
    int current_height = static_cast<int>(m_head_block_height);
    int current_bin_idx = static_cast<int>(m_head_block_height - m_latest_confirmed_block_height) - 1;
    int current_queue_idx = -1;

    for (size_t i = 0; i < m_block_pool[current_bin_idx].size(); ++i) {
      if (m_head_block_id == m_block_pool[current_bin_idx][i].block.getBlockIdB64()) {
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
        target_queue_idx = m_block_pool[target_bin_idx][target_queue_idx].prev_block_vector_idx;
        target_height--;
        target_bin_idx--;

        front_count++;
      } else if (target_height < current_height) {
        current_queue_idx = m_block_pool[current_bin_idx][current_queue_idx].prev_block_vector_idx;
        current_height--;
        current_bin_idx--;

        back_count++;
      }
    }

    while ((target_bin_idx != current_bin_idx) || (target_queue_idx != current_queue_idx)) {
      where_to_go.emplace_front(target_bin_idx, target_queue_idx);
      target_queue_idx = m_block_pool[target_bin_idx][target_queue_idx].prev_block_vector_idx;
      target_height--;
      target_bin_idx--;
      front_count++;

      current_queue_idx = m_block_pool[current_bin_idx][current_queue_idx].prev_block_vector_idx;
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
    } else if (m_block_pool[target_bin_idx][target_queue_idx].block.block_id !=
               m_block_pool[current_bin_idx][current_queue_idx].block.block_id) {
      CLOG(ERROR, "URBK") << "Something error in move_head() - Find common ancestor(2)";
      return;
    }

    // 만남지점까지 back, 그리고 new_head까지 forward하는 함수를 구현
    // 이거 구현하려면 ledger 관련, state tree 관련부터 해야함. 그래야 할 수 있음.

    where_to_go[0].first, where_to_go[0].second; // front를 하기 위한 방향 저장된것.
  }
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
    // data.record_id = item.first;
    data.user_id = item.second[USER_ID];
    data.var_type = item.second[VAR_TYPE];
    data.var_name = item.second[VAR_NAME];
    data.var_value = item.second[VAR_VALUE];
    uint path = (uint)stoul(item.second[PATH]);
    m_user_state_tree.addNode(path, data);
  }

  m_user_state_tree.printTreePostOrder();
}
} // namespace gruut
