//
// Created by ISRL-JH on 2018-12-28.
//

#ifndef WORKSPACE_STORAGE_LIB_HPP
#define WORKSPACE_STORAGE_LIB_HPP

#include <iostream>
#include <stdio.h>
#include <fstream>      // ifstream 클래스
#include <string>
#include <utility>      // pair 클래스
#include <vector>
#include <deque>
#include <map>
#include "json.hpp"
#include "merkle_lib.hpp"
#include "db_lib.hpp"

using namespace std;
using json = nlohmann::json;

typedef unsigned int uint;

enum {USER_ID, VAR_TYPE, VAR_NAME, VAR_VALUE, PATH};
enum {COIN_VALUE=-1, DATA_EXIST=-2};

namespace gruut
{
    // Smart contract 로 부터(?) 전달받은 transaction 들을(블록) 저장해놓는 Block 클래스
    class Block
    {
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
    class Key
    {
    public:
        string user_id;
        string var_type;
        string var_name;

        Key(string id="", string type="", string name="")
        {
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
    ostream& operator<< (ostream& os, Key& key)
    {
        os << key.user_id << ", " << key.var_type << ", " << key.var_name;
        return os;
    }

    // Layer 클래스에 map 변수를 위한 value
    class Value
    {
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
    ostream& operator<<(ostream& os, Value& value)
    {
        os << value.var_value << ", " << value.path;
        return os;
    }


    // Storage 클래스에서 rollback 구현을 위해 갖고 있는 Layer 클래스
    // 각 Layer 에서 수행한 transaction 들과 수행 후의 머클 트리,
    // 그리고 transaction 을 수행한 뒤의 변한 변수들을 map 으로 갖고있음
    class Layer
    {
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

    ostream& operator<<(ostream& os, Layer& layer)
    {
        int t_num = 0;
        os << "Transaction..." << endl;
        for(auto transaction: layer.transaction)
        {
            os << "Transaction #" << t_num << " \t" << transaction << '\n';
            t_num++;
        }
        os << "Temporary_data..." << endl;
        for(auto data: layer.m_temporary_data)
        {
            os << "Key: " << data.first.user_id << ", " << data.first.var_type << ", " << data.first.var_name << ", Value: " << data.second << endl;
        }
        return os;
    }


    class Storage
    {
    private:
        int MAX_LAYER_SIZE;
        deque<Layer> m_layer;
        Layer m_current_layer;
        MerkleTree m_tree;
        mariaDb m_server;

        void readConfig()
        {
            ifstream config_file("/mnt/d/lab/Project/workspace/gruut-storage/lib/storage_config.json");
            if(!config_file) {
                cout << "ifstream error... Please check config file path" << endl;
                return;
            }
            string config_content( (istreambuf_iterator<char>(config_file)), istreambuf_iterator<char>() );
            json js = json::parse(config_content);

            MAX_LAYER_SIZE = js["MAX_LAYER_SIZE"];
        }
        void setupDB()
        {
            gruut::parseJson pJ;

            string serverIp = "127.0.0.1";
            string serverPort = "3307";
            string admin = "root";
            string pw = "1234";
            string db = "thevaulters";

            m_server.setServerIp(&serverIp);
            m_server.setAdmin(&admin);
            m_server.setPassword(&pw);
            m_server.setDatabase(&db);
            m_server.setPort(&serverPort);

            if(m_server.connectionSetup() != 0) {
                cout << "setupDB function failed.." << endl;
                exit(1);
            }
        }
        void setupMerkleTree()
        {
            vector< pair< int, vector<string> > > all = m_server.selectAll();

            for(auto item: all)
            {
                printf("%5d\t", item.first);
                for(auto column: item.second)
                    printf("%15s\t", column.c_str());
                printf("\n");


                test_data data;
                //data.record_id = item.first;
                data.user_id = item.second[USER_ID];
                data.var_type = item.second[VAR_TYPE];
                data.var_name = item.second[VAR_NAME];
                data.var_value = item.second[VAR_VALUE];
                uint path = (uint) stoul(item.second[PATH]);
                m_tree.addNode(path, data);
            }

            m_tree.printTreePostOrder();
        }
        void destroyDB()
        {
            if(m_server.disConnection() != 0) {
                cout << "distroyDB function failed.." << endl;
                exit(1);
            }
        }

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

        void setBlocksByJson()
        {
            ifstream config_file("/mnt/d/lab/Project/workspace/gruut-storage/test/blocks_test.json");
            if(!config_file) {
                cout << "ifstream error... Please check config file path" << endl;
                return;
            }
            string config_content( (istreambuf_iterator<char>(config_file)), istreambuf_iterator<char>() );
            json js = json::parse(config_content);

            Block b_tmp;
            int b_num = 0, t_num = 0;
            for(auto block: js["blocks"])
            {
                b_tmp.clearTransaction();
                cout<<"Block #"<<b_num<<endl;
                t_num = 0;
                for(auto transaction: block["transactions"])
                {
                    cout<<"\tTransaction #" << t_num << "\t" <<transaction<<endl;
                    b_tmp.addTransaction(transaction);
                    t_num++;
                }
                blocks.push_back(b_tmp);
                b_num++;
            }

        }

        Layer popFrontLayer()
        {
            Layer front_layer;
            if (!m_layer.empty()) {
                front_layer = m_layer.front();
                m_layer.pop_front();
            }
            else {
                cout << "[ERROR] popFrontLayer() - storage::m_layer variable is empty!\n";
                exit(1);
            }
            return front_layer;
        }
        Layer popBackLayer()
        {
            Layer back_layer;
            if (!m_layer.empty()) {
                back_layer = m_layer.back();
                m_layer.pop_back();
                // TODO: back layer를 이용해서 머클트리 갱신
            }
            else {
                cout << "[ERROR] storage::m_layer variable is empty!\n";
                exit(1);
            }
            return back_layer;
        }
        void applyFrontLayer()
        {
            // Layer front_layer = popFrontLayer();
            // m_server.setDataByLayer(front_layer);
            // m_tree.setTree(front_layer.m_layer_tree);
            Layer layer = popFrontLayer();
            for(auto data: layer.m_temporary_data)
            {
                cout << data.first.user_id << ", " << data.second << endl;
            }
        }

        void pushLayer() {
            // 꽉 차있으면 Front layer 를 DB 에 반영 시킨 다음 push back 수행
            if (m_layer.size() == MAX_LAYER_SIZE) {
                applyFrontLayer();
            }
            m_layer.push_back(m_current_layer);
        }

        // block 을 한줄씩 읽으며 데이터와 머클트리 갱신, m_current_layer 갱신
        void parseBlockToLayer(Block block)
        {
            m_current_layer.clear();

            int res;
            Value value;
            cout << "parseBlockToLayer function..." << endl;
            for(auto transaction: block.m_transaction)
            {
                cout << transaction << endl;
                if(transaction["command"] == "add")
                {
                    res = addCommand(transaction, value);
                }
                else if (transaction["command"] == "send")
                {
                    res = sendCommand(transaction);

                }
                else if (transaction["command"] == "new")
                {
                    res = newCommand(transaction);
                }
                else if (transaction["command"] == "del")
                {
                    res = delCommand(transaction);
                }
                else
                {
                    cout << "[ERROR] Storage::parseBlockToLayer - can't analyze transaction - " << transaction << endl;
                    continue;
                }

                // command 성공 시
                if (!res)
                {
                    cout << transaction << " Success." << endl;
                }
                else if (res == COIN_VALUE)
                {
                    cout << transaction << " Failed. command make value under zero." << endl;
                }
            }

//            m_current_layer.m_layer_tree;
//            m_current_layer.m_temporary_data;
//            m_current_layer.transaction;

        }


        int addCommand(json transaction, Value &val) {
            string to_user_id = transaction["to_user_id"];
            string to_var_type = transaction["to_var_type"];
            string to_var_name = transaction["to_var_name"];
            int value = transaction["value"];

            Key key(to_user_id, to_var_type, to_var_name);
            map<Key, Value>::iterator it = m_current_layer.m_temporary_data.end();
            int depth = checkLayer(key);

            test_data modified_data;

            if (depth == -2) {
                cout << "[ERROR] Storage::addCommand() - Can't find data" << endl;
                return 1;
            } else if (depth == -1) {
                // DB 연결해서 데이터 읽어온 뒤, add 명령어 반영한 데이터를 m_current_layer 에  저장
                cout << key << " in the DB" << endl;
                pair<int, vector<string> > data = m_server.selectAllUsingUserIdVarTypeVarName(to_user_id, to_var_type,
                                                                                              to_var_name);
                val.var_value = data.second[VAR_VALUE];
                val.path = (uint) stoul(data.second[PATH]);
                val.isDeleted = false;


            } else if (depth == 4) {
                // m_current_layer 에 있는 데이터를 읽어온 뒤, add 명령어 반영한 데이터를 다시 m_current_layer 에 저장
                cout << key << " in the m_current_layer" << endl;

                it = m_current_layer.m_temporary_data.find(key);
                val = it->second;

            } else {
                // m_layer[depth] 에 있는 데이터를 읽어온 뒤, add 명령어 반영한 데이터를 m_current_layer 에 저장
                cout << key << " in the m_layer[" << depth << "]" << endl;

                it = m_layer[depth].m_temporary_data.find(key);
                val = it->second;

            }

            // Q. add 명령어이면 무조건 var_value 는 정수형인가?
            val.var_value = to_string(stoi(val.var_value) + value);

            if (stoi(val.var_value) < 0) {
                return COIN_VALUE;
            }

            //modified_data.record_id = data.first;
            modified_data.user_id = key.user_id;
            modified_data.var_type = key.var_type;
            modified_data.var_name = key.var_name;
            modified_data.var_value = val.var_value;

            m_tree.modifyNode(val.path, modified_data);   // merkle tree 의 노드 수정
            m_current_layer.transaction.push_back(transaction);                 // 반영된 transaction 보관
            // it == end() 이면 DB 에서 처음으로 불러온 데이터인 경우이므로 insert
            if(it == m_current_layer.m_temporary_data.end())
                m_current_layer.m_temporary_data.insert(make_pair(key, val));
            // 그렇지 않으면 m_current_layer 또는 m_layer[depth]에서 찾은 경우이므로 update
            else
                it->second.var_value = val.var_value;

            return 0;
        }
        int sendCommand(json transaction)
        {
            string to_user_id   = transaction["to_user_id"];
            string to_var_type  = transaction["to_var_type"];
            string to_var_name  = transaction["to_var_name"];
            string from_user_id = transaction["from_user_id"];
            string from_var_type  = transaction["to_var_type"];
            string from_var_name= transaction["from_var_name"];
            int value = transaction["value"];

            Key to_key(to_user_id, to_var_type, to_var_name);
            Key from_key(from_user_id, from_var_type, from_var_name);

            int to_depth = checkLayer(to_key);
            int from_depth = checkLayer(from_key);

            return 0;
        }
        int newCommand(json transaction)
        {
            string user_id = transaction["user_id"];
            string var_type = transaction["var_type"];
            string var_name = transaction["var_name"];
            string value = transaction["var_value"];

            Key key(user_id, var_type, var_name);

            int depth = checkLayer(key);

            if(depth == -2) {
                // new_layer 의 map 변수에 새로운 데이터 삽입

            }
            else {
                cout << "[ERROR] Storage::newCommand() - Data already exist" << endl;
                return DATA_EXIST;
            }

            return 0;
        }
        int delCommand(json transaction)
        {

            return 0;
        }


        // 현재 레이어부터 시작해서, 윗 레이어부터 살펴보며 데이터가 존재하면 존재하는 레이어 층을 반환함.
        // 레이어에 없으면 마지막으로 DB를 살펴본 뒤, 존재하면 -1, 존재하지 않으면 -2 반환함.
        int checkLayer(Key key)
        {
            int depth = 4;
            map<Key, Value>::iterator it;

            it = m_current_layer.m_temporary_data.find(key);
            if (it != m_current_layer.m_temporary_data.end()) {
                return depth;
            }

            depth = m_layer.size() - 1;

            for(; depth >= 0; depth--)
            {
                it = m_layer[depth].m_temporary_data.find(key);
                if (it != m_layer[depth].m_temporary_data.end())
                    break;

            }
            if (depth == -1) {
                // DB에 데이터 없음
                if ( m_server.checkUserIdVarName(&key.user_id, &key.var_name) ) {
                    depth = -2;
                }
            }
            return depth;
        }


        void testStorage()
        {
            testForward(blocks[0]);
            testShow("mizno", "coin", "gru");
            testShow("mang", "coin", "gru");
            testForward(blocks[1]);
            testShow("mizno", "coin", "gru");
            testShow("mang", "coin", "gru");
            applyFrontLayer();
//            testForward(blocks[2]);
//            testShow("mizno", "coin", "gru");
//            testShow("kjh", "coin", "gru");
//
//            testBackward(1);
//            testShow("mizno", "coin", "gru");
//            testBackward(1);
//            testShow("mizno", "coin", "gru");
        }
        void testForward(Block block)
        {
            parseBlockToLayer(block);
            pushLayer();
        }
        void testBackward(int back_cnt)
        {
            while(back_cnt--) {
                popBackLayer();
            }
        }
        void testShow(string user_id, string var_type, string var_name)
        {
            // m_layer deque 를 저장한
            deque<Layer> m_layer_temp;
            m_layer_temp.swap(m_layer);

            Key key(user_id, var_type, var_name);
            cout<<"testShow - key: "<<key<<endl;
            Value val;
            map<Key, Value>::iterator it;
            bool found = false;

            while(!m_layer_temp.empty())
            {
                Layer layer = m_layer_temp.back();
                m_layer_temp.pop_back();
                m_layer.push_front(layer);

                it = layer.m_temporary_data.find(key);
                if(it!=layer.m_temporary_data.end()) {
                    val = it->second;
                    cout<<"testShow - val: "<<val<<endl;
                    found = true;
                    break;
                }
            }

            while(!m_layer_temp.empty())
            {
                Layer layer = m_layer_temp.back();
                m_layer_temp.pop_back();
                m_layer.push_front(layer);
            }

            if (!found)
            {
                cout << "maybe data is in the DB..." << endl;
            }
        }
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
