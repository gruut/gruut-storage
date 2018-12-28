//
// Created by ISRL-JH on 2018-12-28.
//

#ifndef WORKSPACE_STORAGE_LIB_HPP
#define WORKSPACE_STORAGE_LIB_HPP

#include <iostream>
#include <fstream>
#include <string>
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
    public:
        storage()
        {
            readConfig();
        }
        void readConfig()
        {
            // init
            ifstream config_file("/mnt/d/lab/Project/workspace/gruut-storage/lib/storage_config.json");
            if(!config_file) {
                cout << "ifstream error... Please check config file path" << endl;
                return;
            }
            string config_content( (istreambuf_iterator<char>(config_file)), istreambuf_iterator<char>() );
            json js = json::parse(config_content);

            MAX_LAYER = js["MAX_LAYER"];
        }
        void test()
        {
        }
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
