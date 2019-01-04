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


namespace gruut
{
    class storage
    {
    private:
        int MAX_LAYER;
        deque<json> m_layer;
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

            MAX_LAYER = js["MAX_LAYER"];
        }
        void setupMerkleTree()
        {
            vector< pair< int, vector<string> > > all = m_server.selectAll();

//            for(auto item: all)
//            {
//                printf("%5d\t", item.first);
//                for(auto column: item.second)
//                    printf("%15s\t", column.c_str());
//                printf("\n");
//            }
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
        void destroyDB()
        {
            if(m_server.disConnection() != 0) {
                cout << "distroyDB function failed.." << endl;
                exit(1);
            }
        }

    public:
        storage()
        {
            readConfig();
            setupDB();
            setupMerkleTree();
        }
        ~storage()
        {
            destroyDB();
        }

    public:
        json blocks;

        void readBlocksJson()
        {
            ifstream config_file("/mnt/d/lab/Project/workspace/gruut-storage/test/blocks_test.json");
            if(!config_file) {
                cout << "ifstream error... Please check config file path" << endl;
                return;
            }
            string config_content( (istreambuf_iterator<char>(config_file)), istreambuf_iterator<char>() );
            json js = json::parse(config_content);

            for(auto tran: js["blocks"]) {
                cout << tran << endl;
            }
            blocks = js["blocks"];
        }

        void testStorage()
        {
            testForward(blocks[0]);
            testShow("mizno");
            testForward(blocks[1]);
            testShow("mizno");
            testForward(blocks[2]);
            testShow("mizno");
            testShow("kjh");

            testBackward(1);
            testShow("mizno");
            testBackward(1);
            testShow("mizno");
        }

        json popLayer()
        {
            json ret = "";
            if (!m_layer.empty()) {
                ret = m_layer.front();
                m_layer.pop_back();
                // TODO: 머클 트리 갱신 (삭제)
            }
            else {
                cout << "[ERROR] storage::m_layer variable is empty!\n";
                exit(1);
            }
            return ret;
        }
        void pushLayer(json transactions)
        {
            m_layer.push_back(transactions);
            // TODO: 머클 트리 갱신 (추가 또는 변경)
        }

        void testForward(json transactions)
        {
            if (m_layer.size() == MAX_LAYER) {
                json apply_layer = popLayer();
                // TODO: DB에 apply_layer 내용 적용시키기
                // parseJson.parse_json_from_smart_contract() (?)
            }
            pushLayer(transactions);
        }
        void testBackward(int back_cnt)
        {
            while(back_cnt--) {
                if (!m_layer.empty()) {
                    m_layer.pop_back();
                }
            }
        }
        void testShow(string user_id)
        {
            deque<json> tmp_layer;


        }
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
