#include <iostream>
#include <cstring>
#include <sstream>
#include <storage_lib.hpp>
#include "json.hpp"
#include "sha256.h"
#include "db_lib.hpp"
#include "merkle_lib.hpp"

using namespace std;

int DB_test() {

    gruut::parseJson pJ;
    gruut::mariaDb mariaDb;

    string serverIp = "127.0.0.1";
    string serverPort = "3307";
    string admin = "root";
    string pw = "1234";
    string db = "thevaulters";
    string smart_contract; // json data from smart contract

    smart_contract = "{\"kind of transaction\":\"money transfer\""
                     ",\"from_user_id\":\"mizno\",\"from_var_name\":\"acc_bal\""
                     ",\"to_user_id\":\"mang\",\"to_var_name\":\"acc_bal\""
                     ",\"value\":10000}";

    cout << smart_contract << endl;
    pJ.parse_json_from_smart_contract(smart_contract);

    /*
    cout << pJ.getPKindOfTransaction() << endl;
    cout << pJ.getPFromUserId() << endl;
    cout << pJ.getPFromVarName() << endl;
    cout << pJ.getPToUserId() << endl;
    cout << pJ.getPToVarName() << endl;
    cout << pJ.getPValue() << endl;
    */

    mariaDb.setServerIp(&serverIp);
    mariaDb.setAdmin(&admin);
    mariaDb.setPassword(&pw);
    mariaDb.setDatabase(&db);
    mariaDb.setPort(&serverPort);

    if(mariaDb.connectionSetup() == 0) {

        // mariaDb.insert("8", "mang", "int", "acc_bal", "150000"); // record_id, user_id, var_name, var_name and var_value of Database
        // mariaDb.update("mizno", "acc_bal", "240000"); // user_id, var_name and var_value of Database
        // mariaDb.deleteData("mang", "acc_bal"); // user_id and var_name of Database
        // mariaDb.selectAllUsingUserId("mizno"); // user_id of Database
        // mariaDb.selectAll();
        // mariaDb.checkUserId("mizno"); // user_id of Database
        // mariaDb.checkVarName("user_name"); // var_name of Database
        // mariaDb.checkUserIdVarName("mizno", "user_name"); // var_name of Database, params should be point variable

        if(pJ.getPKindOfTransaction() == "money transfer") {
            if(mariaDb.transferMoney(pJ.getPFromUserId(), pJ.getPFromVarName(), pJ.getPToUserId(), pJ.getPToVarName(), pJ.getPValue()) == 0) {

            }
        } else {
        }

        if(mariaDb.disConnection()==0) {

        }
    }
    return 0;
}


int main()
{
//    gruut::storage g_storage;
//
//    g_storage.readBlocksJson();
//    g_storage.testStorage();

    gruut::MerkleNode test;
    test.makePath(1, "mizno", "user_name");
    test.makePath(2, "mizno", "bank_name");
    test.makePath(3, "mizno", "btc");
    test.makePath(4, "mizno", "gru");
    test.makePath(5, "mang", "user_name");
    test.makePath(6, "mang", "bank_name");
    test.makePath(7, "mang", "btc");
    test.makePath(8, "mang", "gru");

    return 0;
}