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

            for(auto item: all)
            {
                printf("%5d\t", item.first);
                for(auto column: item.second)
                    printf("%15s\t", column.c_str());
                printf("\n");
            }
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
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
