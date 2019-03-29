//
// Created by msk on 2019-03-25.
//

#ifndef WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP
#define WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP

#include "../utils/types.hpp"
#include "storage_manager.hpp"
#include "../utils/type_converter.hpp"

namespace gruut {

    struct Block {   //  지속적 확인 필요
        block_id_type block_id;
        block_height_type block_height;
        hash_t block_hash;
        timestamp_t block_time;
        block_id_type block_prev_id;
        hash_t block_prev_hash;
        id_type block_prod_id;
        std::vector<hash_t> block_txs;       //  transaction identifiers included this block
        hash_t tx_root;
        hash_t us_state_root;
        hash_t sc_state_root;
        bytes block_raw;       //  block’s raw data (blob type)
    };


    struct UnresolvedBlock {
        int prev_block_vector_idx{-1};
        Block block;

        UnresolvedBlock() = default;
        UnresolvedBlock(Block &block_, int prev_queue_idx_, size_t confirm_level_,
                        bool init_linked_)
                : block(block_), prev_vector_idx(prev_queue_idx_),
                  confirm_level(confirm_level_), linked(init_linked_) {}
    };


    class UnresolvedBlockPool {
    private:
        std::deque<std::vector<UnresolvedBlock>> m_block_pool;      // deque[n] is tree's depth n; vector's blocks are same depth(block height)
        std::recursive_mutex m_push_mutex;

        block_id_type m_latest_confirmed_block_id;
        block_id_type m_latest_confirmed_prev_block_id;
        hash_t m_latest_confirmed_hash;
        hash_t m_latest_confirmed_prev_hash;
        std::atomic<block_height_type> m_latest_confirmed_block_height;
        timestamp_t m_latest_confirmed_block_time;


    public:


    };

}

#endif //WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP