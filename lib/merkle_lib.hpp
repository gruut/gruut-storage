//
// Created by ISRL-JH on 2018-12-03.
//

#ifndef WORKSPACE_MERKLE_LIB_HPP
#define WORKSPACE_MERKLE_LIB_HPP

#include <vector>
#include <string>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <sha256/sha256.h>
using namespace std;

// TODO: define 값 변경
#define _TREE_DEPTH 3
#define _SHA256_SPLIT 2
typedef unsigned int uint;
typedef unsigned long long int ullint;

/////////////////// test data /////////////////////
struct test_data {
    int uid;
    string user_name;
    string var_name;
    string var_type;
    int var_value;
};

test_data null_data = { -1, "0", "0", "0", -1 };
//////////////////////////////////////////////////

namespace gruut {
    class MerkleNode {
    private:
        MerkleNode *m_left;
        MerkleNode *m_right;
        uint m_suffix;    // TODO: 비트 확장
        string m_value;     // TODO: 비트 확장
        uint m_debug_path;      // 노드 분기 시 필요할 것으로 예상됨
        int m_debug_uid;          // 테스트 용도
        int m_suffix_len;
        //MerkleNode *m_next;

        string makeValue(test_data data) {
            string key = to_string(data.uid) + data.user_name + data.var_name + data.var_type + to_string(data.var_value);
            string value = sha256(key);

            return value;
        }
        uint makePath(test_data data) {
            string key = to_string(data.uid) + data.user_name + data.var_type;
            string value = sha256(key).substr(0, _SHA256_SPLIT);
            uint path = (uint) strtoul(value.c_str(), 0, 16);
            uint mask = (1 << _TREE_DEPTH) - 1;

            return path & mask;
        }

    public:
        MerkleNode(test_data data = null_data)
        {
            m_left = nullptr;
            m_right = nullptr;
            //m_next = nullptr;
            m_suffix = 0;
            m_value = makeValue(data);
            //m_path = makePath(data);
            m_debug_path = 0;
            m_debug_uid = data.uid;
            m_suffix_len = -1;
        }

        void reHash()
        {
            string l_value, r_value;

            if (getLeft() != nullptr) {
                l_value = getLeft()->getValue();
            }
            if (getRight() != nullptr) {
                r_value = getRight()->getValue();
            }

            if (l_value.empty()) {
                m_value = sha256(r_value);
            }
            else if (r_value.empty()) {
                m_value = sha256(l_value);
            }
            else {
                m_value = sha256(l_value + r_value);
            }
        }
        // 노드 삭제 시 부모 노드로 이동하여 변경되는 path 와 suffix, suffix_len 의 내용을 반영
        // 1. path 의 LSB 를 저장하고 right shift 수행
        // 2. suffix 의 LSB 에서 suffix_len 번째 비트에 저장된 비트를 넣고 suffix_len 1 증가
        void moveToParent()
        {
            int bit;
            // 예외 처리
            if (m_suffix_len == _TREE_DEPTH) {
                printf("MerkleNode::moveToParent has some error");
                return;
            }

            bit = (m_debug_path & 1) != 0 ? 1 : 0;
            m_debug_path = m_debug_path >> 1;
            m_suffix = m_suffix | (bit << m_suffix_len);
            m_suffix_len++;
        }

        bool isDummy()  { return (m_debug_uid == -1); }
        bool isLeaf()   { return ((m_debug_uid != -1) && (m_suffix_len == 0)); }

        /* setter */
        void setLeft(MerkleNode *node)  { m_left  = node; }
        void setRight(MerkleNode *node) { m_right = node; }
        //void setNext(MerkleNode *node)  { m_next  = node;}
        void setSuffix(uint _path, int pos)
        {
            uint mask = (uint) (1 << pos) - 1;
            m_suffix = _path & mask;
            m_suffix_len = pos;
        }
        void setDebugPath(uint _path)        { m_debug_path  = _path; }
        void setNodeInfo(test_data data)
        {
            m_debug_uid = data.uid;
            m_value = makeValue(data);
        }
        void overwriteNode(MerkleNode *node)
        {
            m_left = nullptr;
            m_right = nullptr;
            m_value = node->getValue();
            m_debug_uid = node->getDebugUid();
            m_suffix = node->getSuffix();
            m_debug_path = node->getDebugPath();
            m_suffix_len = node->getSuffixLen();
            moveToParent();
        }

        /* getter */
        MerkleNode* getLeft()   { return m_left; }
        MerkleNode* getRight()  { return m_right; }
        uint getSuffix()        { return m_suffix; }
        string getValue()       { return m_value; }
        uint getDebugPath()     { return m_debug_path; }
        int getDebugUid()       { return m_debug_uid; }
        int getSuffixLen()      { return m_suffix_len; }
        //MerkleNode* getNext() { return m_next; }

    };






    class MerkleTree {
    private:
        MerkleNode *root;
        ullint m_size;
        stack<MerkleNode *> stk;    // leaf 에서 root 까지의 해쉬 재 계산을 위해 MerkleNode *를 기억해두는 변수

        bool _debug_dir;
        int _debug_depth;
        string _debug_str_depth;
        string _debug_str_dir;

        bool getDirectionOf(uint path, int pos)
        {
            return (path & (1 << pos)) != 0;
        }
        char* intToBin(uint num) {
            char *ret = new char(_SHA256_SPLIT + 2);

            int pos = 0;
            for (int i = _SHA256_SPLIT; i >= 0; --i) {
                if ( (num & (1 << i)) == 0) {
                    ret[pos] = '0';
                }
                else {
                    ret[pos] = '1';
                }
                pos++;
            }
            ret[_SHA256_SPLIT + 1] = '\0';

            return ret;
        }
        void visit(MerkleNode *node) {
            string str_dir = !_debug_dir ? "Left" : "Right";
            if (!node->isDummy()) {
                printf("%s%s\t", _debug_str_dir.substr(0, _debug_depth).c_str(), str_dir.c_str());
                printf("[uid %-3d] path: %s, hash_value: %s\n", node->getDebugUid(), intToBin(node->getDebugPath()), node->getValue().c_str());
            }
            _debug_depth--;
        }
        void postOrder(MerkleNode *node) {
            _debug_depth++;
            printf("%s[depth %3d]\n", _debug_str_depth.substr(0, _debug_depth*2).c_str(), _debug_depth);
            if (node->getLeft() != nullptr) { _debug_dir = false; postOrder(node->getLeft()); }
            if (node->getRight() != nullptr) { _debug_dir = true; postOrder(node->getRight()); }
            visit(node);
        }

    public:
        MerkleTree()
        {
            root = new MerkleNode();
            m_size = 0;
        }

        // TODO: DB 와 연동하여 완성한 이후에는 new_path 파라미터 제거
        void addNode(uint new_path, test_data data) {
            MerkleNode *node = new MerkleNode(data);
            node->setDebugPath(new_path);
            addNode(new_path, node);
        }

        void addNode(uint new_path, MerkleNode *new_node)
        {
            MerkleNode *node = root;
            MerkleNode *prev_node = nullptr;

            bool collision = false;
            bool dir;
            int dir_pos = _TREE_DEPTH - 1;

            while(!stk.empty()) stk.pop();      // 스택 clear
            int depth = 0;
            while(true) {
                if (!node->isDummy()) {
                    collision = true;
                    break;
                }

                stk.push(node); // 노드 삽입 후 노드에서 머클 루트까지 re-hashing 용도

                dir = getDirectionOf(new_path, dir_pos);
                depth++;
                if (!dir && (node->getLeft() == nullptr)) {
                    node->setLeft(new_node);
                    node->setSuffix(new_path, dir_pos);
                    break;
                }
                else if (dir && (node->getRight() == nullptr)) {
                    node->setRight(new_node);
                    node->setSuffix(new_path, dir_pos);
                    break;
                }
                else {
                    if (!dir) {
                        prev_node = node;
                        node = node->getLeft();
                    }
                    else {
                        prev_node = node;
                        node = node->getRight();
                    }
                }

                dir_pos--;
            }
            // 기존 노드와 충돌 났을 때, 기존 노드와 새 노드 모두 삽입 필요
            if (collision) {
                MerkleNode *old_node;
                uint old_path;
                MerkleNode *dummy;

                old_node = node;
                old_path = old_node->getDebugPath();

                // 먼저 충돌난 곳에 dummy 노드 생성해서 연결
                dummy = new MerkleNode();
                stk.push(dummy);
                if (!getDirectionOf(new_path, dir_pos + 1))
                    prev_node->setLeft(dummy);
                else
                    prev_node->setRight(dummy);
                prev_node = dummy;

                // 경로가 같은 횟수만큼 경로를 따라서 dummy 노드 생성 및 연결
                for(int i = dir_pos; i >= 0; --i) {
                    if (getDirectionOf(new_path, i) == getDirectionOf(old_path, i)) {
                        dummy = new MerkleNode();
                        stk.push(dummy);
                        if (!getDirectionOf(new_path, i)) {
                            prev_node->setLeft(dummy);
                        }
                        else {
                            prev_node->setRight(dummy);
                        }
                        prev_node = dummy;
                    }
                    else {
                        if (!getDirectionOf(new_path, i)) {
                            prev_node->setLeft(new_node);
                            prev_node->setRight(old_node);
                        }
                        else {
                            prev_node->setRight(new_node);
                            prev_node->setLeft(old_node);
                        }
                        break;
                    }
                }
            }

            MerkleNode *tmp;
            while(!stk.empty()) {
                tmp = stk.top();
                stk.pop();

                tmp->reHash();
            }

            m_size++;
        }

        void modifyNode(uint path, test_data data)
        {
            MerkleNode *node = getMerkleNode(path);

            node->setNodeInfo(data);
            while(!stk.empty()) {
                node = stk.top();
                stk.pop();

                node->reHash();
            }
        }

        void removeNode(uint path)
        {
            MerkleNode *node;
            MerkleNode *parent, *left, *right;

            node = getMerkleNode(path);
            parent = stk.top(); stk.pop();
            if (parent->getLeft() == node) {
                parent->setLeft(nullptr);
                delete node;
            }
            else if (parent->getRight() == node) {
                parent->setRight(nullptr);
                delete node;
            }

            do {
                if (parent == root) {
                    parent->reHash();
                    break;
                }

                left = parent->getLeft();
                right = parent->getRight();

                if (left != nullptr && right != nullptr)    break;          // left, right 가 모두 존재하면 머클 루트까지 rehash 시작
                else if (left == nullptr)   parent->overwriteNode(right);   // right 만 존재하면 parent 를 right 로 덮어씌움
                else if (right == nullptr)  parent->overwriteNode(left);    // left 만 존재하면 parent 를 left 로 덮어씌움

                if (stk.empty())    break;
                else                parent = stk.top(); stk.pop();
            } while(true);

            while (!stk.empty()) {
                parent->reHash();

                parent = stk.top(); stk.pop();
            }
        }

        MerkleNode* getMerkleNode(uint _path)
        {
            MerkleNode *ret = nullptr;

            while(!stk.empty()) {
                stk.pop();      // 스택 clear
            }

            // _path 따라 내려가면서 dummy 가 아닌 노드 (실제 정보를 가진 노드)를 발견하면 반환
            MerkleNode *node = root;
            int dir_pos = _TREE_DEPTH - 1;
            while(true) {
                //printf("dir pos: %d\t\t", dir_pos);
                //printf("getMerkleNode stack size: %ld\n", stk.size());
                if (!node->isDummy()) {
                    //printf("not dummy\n");
                    ret = node;
                    break;
                }

                if (dir_pos < 0) {
                    break;
                }
                stk.push(node);
                if (!getDirectionOf(_path, dir_pos)) {
                    node = node->getLeft();
                }
                else {
                    node = node->getRight();
                }
                dir_pos--;
            }
            return ret;
        }

        // Debugging
        void printTreePostOrder() {
            ullint size = getSize();
            printf("*********** print tree data by using post-order traversal... ************\n\n");
            printf("node size: %lld\n\n", getSize());
            if (size == 0) {
                printf("there is nothing to printing!!\n\n");
                return;
            }
            _debug_depth = -1;
            _debug_dir = false;
            _debug_str_depth = _debug_str_dir = "";
            for(int i = 0; i < _TREE_DEPTH*2; ++i) {
                _debug_str_depth += " ";
                _debug_str_dir += "\t";
            }
            postOrder(root);
            printf("*********** finish traversal ***********\n");
        }

        // setter
        // void setSize(ullint _size) { m_size = _size; }

        // getter
        ullint getSize()   { return m_size; }
        string getRootValue()   { return root->getValue(); }
        MerkleNode* getRoot()   { return root; }

    };
}

#endif //WORKSPACE_MERKLE_LIB_HPP
