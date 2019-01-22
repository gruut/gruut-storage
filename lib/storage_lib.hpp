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
#include <deque>
#include "json.hpp"
#include "merkle_lib.hpp"
#include "db_lib.hpp"

using namespace std;
using json = nlohmann::json;

typedef unsigned int uint;

namespace gruut
{
    class storage
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
            }
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
        }

        void pushLayer(Layer layer)
        {
            // layer 처음 삽입 시, m_current_layer 에 아무것도 들어있지 않으므로 무시
            if(layer!=nullptr)
            {
                if(m_layer.size() == MAX_LAYER_SIZE)
                {
                    applyFrontLayer();
                }
                m_layer.push_back(layer);

            }
            else
            {
                cout<<"m_current_layer is empty!!"<< endl;
            }
        }
        Layer parseBlockToLayer(Block block)
        {
            return Layer(block);
//
//            json parse_result;
//            json data;
//            m_current_layer.clear();
//            m_current_layer = json::array();
//
//            for(auto transaction: block["transactions"])
//            {
//                cout << transaction.dump(4) << endl;
//                data.clear();
//
//                if (transaction["command"] == "send") {
//                    string from_user_id = transaction["from_user_id"];
//                    string from_var_name = transaction["from_var_name"];
//                    string to_user_id = transaction["to_user_id"];
//                    string to_var_name = transaction["to_var_name"];
//
//                    int from_depth = checkLayer(from_user_id, from_var_name);
//                    int to_depth = checkLayer(to_user_id, to_var_name);
//                    if (from_depth == -1) {
//                        cout << "Send From, 0" << endl;     // ERROR (주는 사람이 존재하지 않음)
//                    }
//                    else if (to_depth == -1) {
//                        cout << "Send To, 0" << endl;       // ERROR (받는 사람이 존재하지 않음)
//                    }
//                    else {
//                        cout << "Send, 1" << endl;
//                    }
//                }
//                else if (transaction["command"] == "add") {
//                    string to_user_id = transaction["to_user_id"];
//                    string to_var_name = transaction["to_var_name"];
//
//                    int depth = checkLayer(to_user_id, to_var_name);
//                    // ERROR (업데이트하려는 행이 존재하지 않음)
//                    if (depth == -1) {
//                        cout << "Add, 0" << endl;
//                    }
//                    // DB에 존재
//                    else if (depth == 0) {
//                        int value = m_server.selectValueUsingUserIdVarName(to_user_id, to_var_name);
//                        data["to_user_id"] = to_user_id;
//                        data["to_var_name"] = to_var_name;
//                        data["value"] = value + transaction.value("value", 0); // value + transaction["value"]
//                        m_current_layer.push_back(data);
//
//                    }
//                    // m_current_layer 또는 m_layer[depth] 에 존재, m_current_layer 업데이트
//                    else {
//                        json found_layer;
//                        if (depth == 4) {
//                            found_layer = m_current_layer;
//                        }
//                        else {
//                            found_layer = m_layer[depth];
//                        }
//                        for (auto k: found_layer) {
//                            if (k["user_id"] == to_user_id && k["var_name"] == to_var_name) {
//                                data["user_id"] = to_user_id;
//                                data["var_name"] = to_var_name;
//                                data["value"] = k.value("value", 0) + transaction.value("value", 0);
//
//                                m_current_layer.update(data);
//                                break;
//                            }
//                        }
//                    }
//                }
//                else if (transaction["command"] == "new") {
//                    string user_id = transaction["user_id"];
//                    string var_name = transaction["var_name"];
//
//                    int depth = checkLayer(user_id, var_name);
//                    if (depth == -1) {
//                        cout << "New, 0" << endl << endl;
//                    }
//                    else {
//                        cout << "New, 1" << endl;   // ERROR (새로 넣으려는 행이 이미 존재)
//                    }
//                }
//                else {
//                        cout << "[ERROR] storage::parseBlock transaction[\"command\"] parameter error" << endl;
//                }
//            }
//
//            return parse_result;
        }
        // current 레이어부터 데이터를 살펴보며 내려가고, 존재하면 존재하는 레이어 층을 반환함.
        // 레이어에 없으면 마지막으로 DB를 살펴본 뒤, 존재하면 0, 존재하지 않으면 -1 반환함.
        int checkLayer(string user_id, string var_name)
        {
            int depth = 4;

            depth = m_layer.size();
            for(auto layer = m_layer.crbegin(); layer != m_layer.crend(); layer++)
            {
                for (auto transaction: layer->transaction) {
                    // 해당 레이어에 찾는 데이터가 있는지 체크
                }
                depth--;
            }
            if (depth == 0) {
                // DB에 데이터 있음
                if ( !m_server.checkUserIdVarName(&user_id, &var_name) ) {
                    depth = 0;
                }
                else {  // 데이터 없음
                    depth = -1;
                }
            }
            return depth;
        }

        void testStorage()
        {
            testForward(blocks[0]);
            testShow("mizno");
//            testForward(blocks[1]);
//            testShow("mizno");
//            testForward(blocks[2]);
//            testShow("mizno");
//            testShow("kjh");
//
//            testBackward(1);
//            testShow("mizno");
//            testBackward(1);
//            testShow("mizno");
        }
        void testForward(Block block)
        {
            pushLayer(m_current_layer);
            m_current_layer = parseBlockToLayer(block);
        }
        void testBackward(int back_cnt)
        {
            while(back_cnt--) {
                popBackLayer();
            }
        }
        void testShow(string user_id)
        {
            // m_layer queue 에서 popBackLayer 하면서 살펴보고, 살펴본 레이어는 tmp_m_layer 에 저장해둔 뒤,
            // 할 일이 끝나면 다시 m_layer 에 넣어둠
        }
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
