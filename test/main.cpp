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
