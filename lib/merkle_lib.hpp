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
        uint m_path;      // 노드 분기 시 필요할 것으로 예상됨
        int m_uid;          // 테스트 용도
        int m_suffix_len;
        //MerkleNode *m_next;

        string makeValue(test_data data) {
            string key = to_string(data.uid) + data.user_name + data.var_name + data.var_type + to_string(data.var_value);
            string value = sha256(key);

            return value;
        }
        uint makePath(test_data data) {
            string key = to_string(data.uid) + data.user_name + data.var_type;
            string path = sha256(key);

            path = path.substr(0, _SHA256_SPLIT);
            return (uint) strtoul(path.c_str(), 0, 16);
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
            m_path = 0;
            m_uid = data.uid;
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

        bool isDummy()  { return (m_uid == -1); }
        bool isLeaf()   { return ((m_uid != -1) && (m_suffix_len == 0)); }

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
        void setPath(uint _path)        { m_path  = _path; }
        void setUid(int _uid)        { m_uid  = _uid; }

        /* getter */
        MerkleNode* getLeft()   { return m_left; }
        MerkleNode* getRight()  { return m_right; }
        uint getSuffix()    { return m_suffix; }
        string getValue()     { return m_value; }
        uint getPath()      { return m_path; }
        int getUid()        { return m_uid; }
        int getSuffixLen()  { return m_suffix_len; }
        //MerkleNode* getNext() { return m_next; }

    };






    class MerkleTree {
    private:
        MerkleNode *root;
        ullint m_size;

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
            //printf("2");
            string str_dir = !_debug_dir ? "Left" : "Right";
            if (!node->isDummy()) {
                printf("%s%s\t", _debug_str_dir.substr(0, _debug_depth).c_str(), str_dir.c_str());
                printf("[uid %-3d] path: %s\n", node->getUid(), intToBin(node->getPath()));
            }
            _debug_depth--;
        }
        void postOrder(MerkleNode *node) {
            _debug_depth++;
            //printf("1");
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
            node->setPath(new_path);
            addNode(new_path, node);
        }

        void addNode(uint new_path, MerkleNode *new_node)
        {
            MerkleNode *node = root;
            MerkleNode *prev_node = nullptr;
            stack<MerkleNode *> stk;

            bool collision = false;
            bool dir;
            int dir_pos = _TREE_DEPTH - 1;

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
                old_path = old_node->getPath();

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

        void modifyNode(uint path, MerkleNode *node)
        {

        }
        void removeNode(uint path)
        {

        }

        MerkleNode* getMerkleNode(uint _path, string _value)
        {
            MerkleNode *node = root;
            int depth = 0;
            int _path_pos = _TREE_DEPTH;

            while(true) {
                if (depth > _TREE_DEPTH){
                    return nullptr;
                }
                else if (node->getValue() == _value) {
                    break;
                }

                // _path 를 보고 left, right 노드로 진행
                if (!getDirectionOf(_path, _path_pos))    { node = node->getLeft(); }
                else                        { node = node->getRight(); }
                depth++;
                _path_pos--;
            }

            return node;
        }

        // Debugging
        void printTreePostOrder() {
            printf("*********** print tree data by using post-order traversal... ************\n\n");
            printf("node size: %lld\n\n", getSize());
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
        void setSize(ullint _size) { m_size = _size; }

        // getter
        ullint getSize()   { return m_size; }
    };
}

#endif //WORKSPACE_MERKLE_LIB_HPP
