#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <iostream>
#include <json.hpp>
#include "../lib/storage_lib.hpp"

using namespace std;

int main() {


    string smart_contract; // json data from smart contract

    smart_contract = "{\"query\":\"update\",\"user_id\":\"mizno\",\"var_name\":\"acc_bal\",\"var_value\":\"180000\"}";

    gruut::mariaDb mariaDb;

    string serverIp = "127.0.0.1";
    string serverPort = "3307";
    string admin = "root";
    string pw = "1234";
    string db = "thevaulters";

    mariaDb.setServerIp(&serverIp);
    mariaDb.setAdmin(&admin);
    mariaDb.setPassword(&pw);
    mariaDb.setDatabase(&db);
    mariaDb.setPort(&serverPort);

    if(mariaDb.connectionSetup() == 0) {
        // mariaDb.insert("8", "mang", "int", "acc_bal", "150000"); // records_id, user_id, var_type, var_name, var_value of Database
        mariaDb.update("mizno", "acc_bal", "200000"); // user_id, var_name, var_value of Database
        // mariaDb.deleteData("mang", "acc_bal"); // user_id, var_name of Database
        // mariaDb.selectUsingUserId("mizno"); // user_id of Database
        // mariaDb.selectAll();
        // mariaDb.checkUserId("mizno"); // user_id of Database

        if(mariaDb.disConnection()==0) {

        }
    }


    return 0;
}
