#include <cstring>

#include "../storage/db_lib.hpp"
#include "../storage/state_merkle_tree.hpp"
#include "../storage/storage.hpp"

using namespace std;

int DB_test() {

  gruut::parseJson pJ;
  gruut::mariaDb mariaDb;

  string serverIp = "127.0.0.1";
  string serverPort = "3306";
  string admin = "gruut_user";
  string pw = "1234";
  string db = "thevaulters";
  string smart_contract; // json data from smart contract

  smart_contract = "{\"kind_of_transaction\":\"money_transfer\""
                   ",\"from_user_id\":\"mizno\",\"from_var_type\":\"coin\",\"from_var_name\":\"gru\""
                   ",\"to_user_id\":\"mang\",\"to_var_type\":\"coin\",\"to_var_name\":\"gru\""
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

  if (mariaDb.connectionSetup() == 0) {

    // test the ledger table
    // mariaDb.insert("8", "temp", "coin", "gru", "150", "11246"); // block_id, user_id, var_name, var_name, merkle_path and var_value of
    // Database mariaDb.updateVarValue("mizno", "coin", "eth", "400"); // user_id, var_name and var_value of Database
    // mariaDb.deleteData("mang", "acc_bal"); // user_id and var_name of Database
    // mariaDb.selectAllUsingUserId("mizno"); // user_id of Database
    // mariaDb.selectAll();
    // mariaDb.checkUserId("mizno"); // user_id of Database
    // mariaDb.checkVarName("user_name"); // var_name of Database
    // mariaDb.checkUserIdVarName("mizno", "user_name"); // var_name of Database, params should be point variable
    /*
    if(pJ.getPKindOfTransaction() == "money_transfer") {
        if(mariaDb.transferMoney(pJ.getPFromUserId(), pJ.getPFromVarType(), pJ.getPFromVarName(), pJ.getPToUserId(), pJ.getPToVarType(),
    pJ.getPToVarName(), pJ.getPValue()) == 0) {

        }
    } else {
    }
    */

    // test the layer table
    // mariaDb.layerInsert("1",
    // "{\"kind_of_transaction\":\"money_transfer\",\"from_user_id\":\"temp\",\"from_var_type\":\"coin\",\"from_var_name\":\"gru\",\"to_user_id\":\"mizno\",\"to_var_type\":\"coin\",\"to_var_name\":\"gru\",\"value\":2}");
    // mariaDb.layerDelete("1",
    // "{\"kind_of_transaction\":\"money_transfer\",\"from_user_id\":\"temp\",\"from_var_type\":\"coin\",\"from_var_name\":\"gru\",\"to_user_id\":\"mizno\",\"to_var_type\":\"coin\",\"to_var_name\":\"gru\",\"value\":2}");
    // mariaDb.layerSelectAll();

    // test the block table
    // mariaDb.blockInsert("1", "cf23df2207d99a74fbe169e3eba035e633b65d94");
    // mariaDb.blockUpdateBlockHash("1", "cf23df2207d99a74fbe169e3eba035e633b65d95");
    // mariaDb.blockDelete("1");
    // mariaDb.blockSelectUsingBlockId("1");
    // mariaDb.blockSelectAll();

    // test the transaction table
    // mariaDb.transactionInsert("2", "req",
    // "{\"kind_of_transaction\":\"money_transfer\",\"from_user_id\":\"mizno\",\"from_var_type\":\"coin\",\"from_var_name\":\"gru\",\"to_user_id\":\"mang\",\"to_var_type\":\"coin\",\"to_var_name\":\"gru\",\"value\":500}");
    // mariaDb.transactionDelete("1", "req",
    // "{\"kind_of_transaction\":\"money_transfer\",\"from_user_id\":\"mizno\",\"from_var_type\":\"coin\",\"from_var_name\":\"gru\",\"to_user_id\":\"mang\",\"to_var_type\":\"coin\",\"to_var_name\":\"gru\",\"value\":10000}");
    // mariaDb.transactionSelectUsingBlockId("2");
    mariaDb.transactionSelectAll();

    if (mariaDb.disConnection() == 0) {
    }
  }
  return 0;
}

int main() {
  gruut::Storage g_storage;

  g_storage.setBlocksByJson();
  g_storage.testStorage();

  //    gruut::StateMerkleNode test;
  //    test.makePath("mizno", "string", "user_name");
  //    test.makePath("mizno", "coin", "eth");
  //    test.makePath("mizno", "coin", "btc");
  //    test.makePath("mizno", "coin", "gru");
  //    test.makePath("mang", "string", "user_name");
  //    test.makePath("mang", "coin", "eth");
  //    test.makePath("mang", "coin", "btc");
  //    test.makePath("mang", "coin", "gru");

  DB_test();

  return 0;
}