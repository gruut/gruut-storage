//
// Created by CH on 2018-11-22.
//

#ifndef WORKSPACE_DB_LIB_HPP
#define WORKSPACE_DB_LIB_HPP

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <iostream>
#include <utility>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

namespace gruut {

    class parseJson {
    private:
        // for money transfer
        string pKindOfTransaction; // p means 'parsed'
        string pFromUserId;
        string pFromVarType;
        string pFromVarName;
        string pToUserId;
        string pToVarType;
        string pToVarName;
        int pValue;

    public:
        void parse_json_from_smart_contract(string json_data);

        string getPKindOfTransaction() { return pKindOfTransaction; }
        string getPFromUserId() { return pFromUserId; }
        string getPFromVarType() { return pFromVarType; }
        string getPFromVarName() { return pFromVarName; }
        string getPToUserId() { return pToUserId; }
        string getPToVarType() { return pToVarType; }
        string getPToVarName() { return pToVarName; }
        int getPValue() { return pValue; }
    };


    class dbSetting {
    protected:
        string serverIp;
        string admin;
        string password;
        string database;
        string port;
        string query;

    public:
        dbSetting() {
            cout << "*** dbSetting() is called" << endl;
        }

        ~dbSetting() {
            cout << "*** ~dbSetting() is called" << endl;
        }

        void setServerIp(string *param) { serverIp = *param; }
        string getServerIp() { return serverIp; }

        void setAdmin(string *param) { admin = *param; }
        string getAdmin() { return admin; }

        void setPassword(string *param) { password = *param; }
        string getPassword() { return password; }

        void setDatabase(string *param) { database = *param; }
        string getDatabase() { return database; }

        void setPort(string *param) { port = *param; }
        string getPort() { return port; }

        int connectionSetup() { return 0; }
        int performQuery(string query) { return 0; }
        int disConnection() { return 0; }

        // for ledger table in DB
        int insert(string blockId, string userId, string varType, string varName, string varValue,
                   string path) { return 0; }
        int updateVarValue(string userId, string varType, string varName, string varValue) { return 0; }
        int deleteData(string userId, string varType, string varName) { return 0; }

        int selectAllUsingUserId(string userId) { return 0; }
        pair<int, vector<string>> selectAllUsingUserIdVarTypeVarName(string userId, string varType, string varName) {
            pair<int, vector<string>> rst;
            return rst;
        }
        int selectValueUsingUserIdVarName(string userId, string varName) { return 0; }
        vector<pair<int, vector<string>>> selectAll() {
            vector<pair<int, vector<string>>> rst;
            return rst;
        }

        // for ledger table in DB
        int checkUserId(string userId) { return 0; }
        int checkVarName(string varName) { return 0; }
        int checkUserIdVarName(string *userId, string *varName) { return 0; }
        int checkUserIdVarTypeVarName(string *userId, string *varType, string *varName) { return 0; }

        // for money transfer
        int transferMoney(string fromUserId, string fromVarType, string fromVarName, string toUserId, string toVarType,
                          string toVarName, int value) { return 0; }
        int checkBalance(string *userId, string *varName, int *value) { return 0; }

        // for layer table in DB, don't need a update function.
        int layerInsert(string blockId, string scContents) { return 0; }
        int layerDelete(string blockId, string scContents) { return 0; }
        int layerSelectAll() { return 0; }

        // for block table in DB
        int blockInsert(string blockId, string blockHash) { return 0; }
        int blockUpdateBlockHash(string blockId, string blockHash) { return 0; }
        int blockDelete(string blockId, string blockHash) { return 0; }
        int blockSelectUsingBlockId(string blockId) { return 0; }
        int blockSelectAll() { return 0; }

        // for transaction table in DB, don't need a update function.
        int transactionInsert(string blockId, string reqrst, string scContents) { return 0; }
        int transactionDelete(string blockId, string reqrst, string scContents) { return 0; }
        int transactionSelectUsingBlockId(string blockId) { return 0; }
        int transactionSelectAll() { return 0; }
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    class mariaDb: public dbSetting {

    private:
        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
        int columns;
        int i;

    public:
        mariaDb() {
            cout << "*** mariaDb() is called" << endl;
        }

        ~mariaDb() {
            cout << "*** ~mariaDb() is called" << endl;
        }

        int connectionSetup();
        int performQuery(string query);
        int disConnection();

        int insert(string blockId, string userId, string varType, string varName, string varValue, string path);
        int updateVarValue(string userId, string varType, string varName, string varValue);
        int deleteData(string userId, string varType, string varName);

        int selectAllUsingUserId(string userId);
        pair<int, vector<string>> selectAllUsingUserIdVarTypeVarName(string userId, string varType, string varName);
        int selectValueUsingUserIdVarName(string userId, string varName);
        vector<pair<int, vector<string>>> selectAll();

        int checkUserId(string userId);
        int checkVarName(string varName);
        int checkUserIdVarName(string *userId, string *varName);
        int checkUserIdVarTypeVarName(string *userId, string *varType, string *varName);

        int transferMoney(string fromUserId, string fromVarType, string fromVarName, string toUserId, string toVarType, string toVarName, int value);
        int checkBalance(string *userId, string *varName, int *value);
        int layerInsert(string blockId, string scContents);
        int layerDelete(string blockId, string scContents);
        int layerSelectAll();

        int blockInsert(string blockId, string blockHash);
        int blockUpdateBlockHash(string blockId, string blockHash);
        int blockDelete(string blockId);
        int blockSelectUsingBlockId(string blockId);
        int blockSelectAll();

        int transactionInsert(string blockId, string reqrst, string scContents);
        int transactionDelete(string blockId, string reqrst, string scContents);
        int transactionSelectUsingBlockId(string blockId);
        int transactionSelectAll();
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    class mysqlDb: public dbSetting {

    private:
        int temp;

    public:
        mysqlDb() {
            cout << "*** mysqlDb() is called" << endl;
        }
        ~mysqlDb() {
            cout << "*** ~mysqlDb() is called" << endl;
        }
    };
}

#endif //WORKSPACE_DB_LIB_HPP
