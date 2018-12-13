#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <iostream>
#include <json.hpp>
#include "../lib/storage_lib.hpp"

using namespace std;

int main() {

    gruut::StorageLib s_lib;
    string smart_contract; // json data from smart contract

    s_lib.dbConnect();
    if(s_lib.getProblem() == 1) {
        return 0;
    }

    smart_contract = "{\"query\":\"update\",\"user_id\":\"mizno\",\"var_name\":\"acc_bal\",\"var_value\":\"180000\"}";

    // s_lib.funcInsert("8", "mang", "int", "acc_bal", "150000");
    // s_lib.funcUpdate("mizno", "account_balance", "200000");
    // s_lib.funcDelete("mang", "acc_bal");

    s_lib.selectAll();

    return 0;


}
