//
// Created by msk on 2019-03-25.
//

#ifndef WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP
#define WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP

#include <deque>
#include <atomic>

#include "../chain/types.hpp"
#include "../utils/type_converter.hpp"

#include "../chain/block.hpp"
#include "storage_manager.hpp"
#include "state_merkle_tree.hpp"

namespace gruut {

    struct UnresolvedBlock {
        int prev_block_vector_idx{-1};
        Block block;

        UnresolvedBlock() = default;
        UnresolvedBlock(Block &block_, int prev_queue_idx_)
                : block(block_), prev_block_vector_idx(prev_queue_idx_) {}
    };


    class UnresolvedBlockPool {
    private:
        std::deque<std::vector<UnresolvedBlock>> m_block_pool;      // deque[n] is tree's depth n; vector's blocks are same depth(block height)
        std::recursive_mutex m_push_mutex;

        StateMerkleTree m_state_tree;        // state tree

        block_id_type m_latest_confirmed_block_id;
        hash_t m_latest_confirmed_hash;
        std::atomic<block_height_type> m_latest_confirmed_block_height;
        timestamp_t m_latest_confirmed_block_time;

        block_id_type m_latest_confirmed_prev_block_id;
        hash_t m_latest_confirmed_prev_hash;

        block_id_type m_head_block_id;
        std::atomic<block_height_type> m_head_block_height;

        StorageManager *m_storage_manager;

    public:
        UnresolvedBlockPool();
        inline size_t size() { return m_block_pool.size(); }
        inline bool empty() { return m_block_pool.empty(); }
        inline void clear() { m_block_pool.clear(); }
        void setPool(const block_id_type &last_block_id,
                     const block_id_type &prev_block_id, const hash_t &last_hash,
                     const hash_t &prev_hash, block_height_type last_height,
                     timestamp_t last_time);
        bool prepareBins(block_height_type t_height);
        void invalidateCaches();
        unblk_push_result_type push(Block &block, bool is_restore = false);
        bool getBlock(block_height_type t_height, const hash_t &t_prev_hash,
                      const hash_t &t_hash, Block &ret_block);
        bool getBlock(block_height_type t_height, Block &ret_block);
        void getResolvedBlocks(std::vector<UnresolvedBlock> &resolved_blocks,
                               std::vector<std::string> &drop_blocks);
        nth_link_type getUnresolvedLowestLink();
        block_layer_t getBlockLayer(const std::string &block_id_b64);
        nth_link_type getMostPossibleLink();
        block_layer_t getMostPossibleBlockLayer();
        bool hasUnresolvedBlocks();
        void restorePool();

        void setupStateTree();

        void process_tx_result(const Block &new_block, json result);
        void move_head(const std::string &block_id_b64, const block_height_type target_block_height);

    private:
        block_layer_t forwardBlockToLedgerAt(int bin_idx, int vector_idx);
        void forwardBlocksToLedgerFrom(int bin_idx, int vector_idx);
        json readBackupIds();
        void backupPool();
        BlockPosOnMap getLongestBlockPos();
        void updateConfirmLevel();
    };

}

#endif //WORKSPACE_UNRESOLVED_BLOCK_POOL_HPP