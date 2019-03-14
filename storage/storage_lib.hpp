//
// Created by ISRL-JH on 2018-12-28.
//

#ifndef WORKSPACE_STORAGE_LIB_HPP
#define WORKSPACE_STORAGE_LIB_HPP

#include <fstream>      // ifstream 클래스
#include <deque>
#include <map>

#include "merkle_lib.hpp"
#include "db_lib.hpp"

using namespace std;
using json = nlohmann::json;

typedef unsigned int uint;

#define _D_CUR_LAYER 4
enum {BLOCK_ID, USER_ID, VAR_TYPE, VAR_NAME, VAR_VALUE, PATH};
enum {SUCCESS=0, COIN_VALUE=-1, DATA_DUPLICATE=-2, DATA_NOT_EXIST=-3};
enum {NO_DATA=-2, DB_DATA=-1, CUR_DATA=_D_CUR_LAYER};

namespace gruut {

    // Smart contract 로 부터(?) 전달받은 transaction 들을(블록) 저장해놓는 Block 클래스
    class Block {
    public:
        vector<json> m_transaction;

        Block()
        {
        }

        void addTransaction(json new_transaction)
        {
            m_transaction.push_back(new_transaction);
        }
        void clearTransaction()
        {
            m_transaction.clear();
        }
    };


    // Layer 클래스에 map 변수를 위한 key
    class Key {
    public:
        string block_id;
        string user_id;
        string var_type;
        string var_name;

        Key(string bId="", string id="", string type="", string name="")
        {
            block_id = bId;
            user_id = id;
            var_type = type;
            var_name = name;
        }
        bool operator<(const Key& key) const
        {
            int user_id_cmp = this->user_id.compare(key.user_id);
            if (user_id_cmp == 0) {
                int var_type_cmp = this->var_type.compare(key.var_type);
                if (var_type_cmp == 0) {
                    int var_name_cmp = this->var_name.compare(key.var_name);
                    return var_name_cmp < 0;
                }
                return var_type_cmp < 0;
            }
            return user_id_cmp < 0;
        }
        friend ostream& operator<< (ostream& os, Key& key);
    };

    ostream& operator<< (ostream& os, Key& key);
    ostream& operator<< (ostream& os, const Key& key);

    // Layer 클래스에 map 변수를 위한 value
    class Value {
    public:
        string var_value;
        uint path;
        bool isDeleted;

        Value()
        {
            var_value = "";
            path = 0;
            bool isDeleted = false;
        }
        Value(string value, uint _path, bool del=false)
        {
            var_value = value;
            path = _path;
            isDeleted = del;
        }
        friend ostream& operator<<(ostream& os, Value& value);
    };

    ostream& operator<<(ostream& os, Value& value);


    // Storage 클래스에서 rollback 구현을 위해 갖고 있는 Layer 클래스
    // 각 Layer 에서 수행한 transaction 들과 수행 후의 머클 트리,
    // 그리고 transaction 을 수행한 뒤의 변한 변수들을 map 으로 갖고있음
    class Layer {
    public:
        vector<json> transaction;
        map<Key, Value> m_temporary_data;   // 레이어가 갖고있는 임시 데이터. MAX_LAYER_SIZE가 넘으면 DB에 반영될 데이터임.

        Layer()
        {
        }
        Layer(Block block)
        {
            cout << "Layer constructor" << endl;
            for(auto transaction: block.m_transaction)
            {
                cout << transaction << endl;
            }
        }
        void clear()
        {
            transaction.clear();
            m_temporary_data.clear();
        }
        friend ostream& operator<<(ostream& os, Layer& layer);
    };

    ostream& operator<<(ostream& os, Layer& layer);


    class Storage {
    private:
        int MAX_LAYER_SIZE;
        deque<Layer> m_layer;
        Layer m_current_layer;
        MerkleTree m_tree;
        mariaDb m_server;

        void readConfig();
        void setupDB();
        void setupMerkleTree();
        void destroyDB();

    public:
        vector<Block> blocks;

        Storage()
        {
            readConfig();
            setupDB();
            setupMerkleTree();
        }
        ~Storage()
        {
            destroyDB();
        }

        void setBlocksByJson();
        void pushLayer();
        Layer popFrontLayer();
        Layer popBackLayer();
        void applyFrontLayer();

        void parseBlockToLayer(Block block);
        int addCommand(json transaction, Value &val);
        int sendCommand(json transaction);
        int newCommand(json transaction);
        int delCommand(json transaction);
        int checkLayer(Key key);

        void testStorage();
        void testForward(Block block);
        void testBackward();
        void testShow(string user_id, string var_type, string var_name);
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
