//
// Created by ISRL-JH on 2018-12-03.
//

#ifndef WORKSPACE_MERKLE_LIB_HPP
#define WORKSPACE_MERKLE_LIB_HPP

#include <vector>
#include <stdlib.h>
#include <sha256/sha256.h>

// TODO: _TREE_DEPTH 값을 64, 또는 128, 256 으로 변경
#define _TREE_DEPTH 8
typedef unsigned int uint;

namespace gruut {
    class MerkleNode {
    private:
        MerkleNode *m_left;
        MerkleNode *m_right;
        uint m_suffix;    // TODO: 비트 확장
        uint m_value;     // TODO: 비트 확장
    public:
        MerkleNode();
    };

    class MerkleTree {
    private:
        MerkleNode *root;
        uint m_path;      // path 를 가져와서 루트에서 탐색

        void hashing(MerkleNode &node);
    public:
        MerkleTree();
        void addNode();
        void modifyNode();

    };
}

#endif //WORKSPACE_MERKLE_LIB_HPP
