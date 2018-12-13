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

    mariaDb.setServerIp("127.0.0.1");
    mariaDb.setAdmin("root");
    mariaDb.setPassword("1234");
    mariaDb.setDatabase("thevaulters");
    mariaDb.setPort("3307");

    mariaDb.connectionSetup();

    // mariaDb.funcInsert("8", "mang", "int", "acc_bal", "150000");
    mariaDb.funcUpdate("mizno", "acc_bal", "210000");
    // mariaDb.funcDelete("mang", "acc_bal");
    mariaDb.selectAll();

    mariaDb.disConnection();

    return 0;
}
