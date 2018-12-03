#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <iostream>
#include <json.hpp>
#include <storage_lib.hpp>
#include <merkle_lib.hpp>
#include <sha256/sha256.h>

using namespace std;

json js[7] = {json::parse("{\"user_id\":\"mizno\", \"var_name\":\"balance\",\"value\":50000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"kjh\", \"var_name\":\"balance\",\"value\":20000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"mang\", \"var_name\":\"balance\",\"value\":30000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"skmoon\", \"var_name\":\"balance\",\"value\":40000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"sheperd\", \"var_name\":\"balance\",\"value\":40000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"sunnyq\", \"var_name\":\"balance\",\"value\":50000,\"type\":\"gru\"}"),
              json::parse("{\"user_id\":\"lhs\", \"var_name\":\"balance\",\"value\":30000,\"type\":\"gru\"}"), };

int main() {
    string input = "grape";
    string output1 = sha256(input);

    cout << "sha256('"<< input << "'):" << output1 << endl;
    return 0;
}
/*
int main() {

    gruut::StorageLib s_lib;
    string smart_contract; // json data from smart contract
    int amount;
    string from, to, type;

    s_lib.dbInit();
    if(s_lib.getProblem() == 1) {
        return 0;
    }

    smart_contract = "{\"amount\":10000,\"from\":\"mizno\",\"to\":\"mang\",\"type\":\"gru\"}";

    s_lib.parseJsonFromSmartContract(smart_contract);

    amount = s_lib.getAmount();
    from = s_lib.getFrom();
    to = s_lib.getTo();
    type = s_lib.getType();

    cout << endl;

    // check user
    s_lib.checkUserId(from);
    s_lib.checkUserId(to);
    if(s_lib.getProblem() == 1) {
        return 0;
    }

    cout << endl;

    // check user's balance and transfer is possible
    s_lib.checkUserBalance(from, "from");
    s_lib.checkUserBalance(to, "to");
    if(s_lib.getProblem() == 1) {
        return 0;
    }

    cout << endl;

    // update the user_balance in DB
    s_lib.updateUserBalance(from, "from");
    s_lib.updateUserBalance(to, "to");

    cout << "*** processed" << endl;
    cout << endl;

    // check user's balance
    s_lib.checkUserBalance(from, "just check");
    s_lib.checkUserBalance(to, "just check");

    return 0;


}
*/