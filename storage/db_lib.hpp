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
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace gruut {

    class parseJson{
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
        void parse_json_from_smart_contract(string json_data) {
            cout << "parse_json_from_smart_contract is called!!!" << endl;
            json js;
            js = json::parse(json_data);

            pKindOfTransaction = js["kind_of_transaction"];
            pFromUserId = js["from_user_id"];
            pFromVarType = js["from_var_type"];
            pFromVarName = js["from_var_name"];
            pToUserId = js["to_user_id"];
            pToVarType = js["to_var_type"];
            pToVarName = js["to_var_name"];
            pValue = js["value"];

            /*
            cout << pKindOfTransaction << " / "
            << pFromUserId << " / " << pFromVarName << " / "
            << pToUserId << " / " << pToVarName << " / "
            << pValue << endl;
            */

        }

        string getPKindOfTransaction() {
            return pKindOfTransaction;
        }

        string getPFromUserId() {
            return pFromUserId;
        }

        string getPFromVarType() {
            return pFromVarType;
        }

        string getPFromVarName() {
            return pFromVarName;
        }

        string getPToUserId() {
            return pToUserId;
        }

        string getPToVarType() {
            return pToVarType;
        }

        string getPToVarName() {
            return pToVarName;
        }

        int getPValue() {
            return pValue;
        }

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

        void setServerIp(string *param) {serverIp = *param;}
        string getServerIp() {return serverIp;}

        void setAdmin(string *param) {admin = *param;}
        string getAdmin() {return admin;}

        void setPassword(string *param) {password = *param;}
        string getPassword() {return password;}

        void setDatabase(string *param) {database = *param;}
        string getDatabase() {return database;}

        void setPort(string *param) {port = *param;}
        string getPort() {return port;}

        int connectionSetup() {return 0;}
        int performQuery(string query) {return 0;}
        int disConnection() {return 0;}

        // for ledger table in DB
        int insert(string blockId, string userId, string varType, string varName, string varValue, string path) {return 0;}
        int updateVarValue(string userId, string varType, string varName, string varValue) {return 0;}
        int deleteData(string userId, string varType, string varName) {return 0;}
        int selectAllUsingUserId(string userId) {return 0;}
        pair<int, vector<string>> selectAllUsingUserIdVarTypeVarName(string userId, string varType, string varName) {pair<int, vector<string>> rst; return rst;}
        int selectValueUsingUserIdVarName(string userId, string varName) {return 0;}
        vector<pair<int, vector<string>>> selectAll() {vector<pair<int, vector<string>>> rst; return rst;}

        // for ledger table in DB
        int checkUserId(string userId) {return 0;}
        int checkVarName(string varName) {return 0;}
        int checkUserIdVarName(string *userId, string *varName) {return 0;}
        int checkUserIdVarTypeVarName(string *userId, string *varType, string *varName) {return 0;}

        // for money transfer
        int transferMoney(string fromUserId, string fromVarType, string fromVarName, string toUserId, string toVarType, string toVarName, int value)  {return 0;}
        int checkBalance(string *userId, string *varName, int *value) {return 0;}

        // for layer table in DB, don't need a update function.
        int layerInsert(string blockId, string scContents) {return 0;}
        int layerDelete(string blockId, string scContents) {return 0;}
        int layerSelectAll() {return 0;}

        // for block table in DB
        int blockInsert(string blockId, string blockHash) {return 0;}
        int blockUpdateBlockHash(string blockId, string blockHash) {return 0;}
        int blockDelete(string blockId, string blockHash) {return 0;}
        int blockSelectUsingBlockId(string blockId) {return 0;}
        int blockSelectAll() {return 0;}

        // for transaction table in DB, don't need a update function.
        int transactionInsert(string blockId, string reqrst, string scContents) {return 0;}
        int transactionDelete(string blockId, string reqrst, string scContents) {return 0;}
        int transactionSelectUsingBlockId(string blockId) {return 0;}
        int transactionSelectAll() {return 0;}


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

        int connectionSetup() {
            MYSQL *connection = mysql_init(NULL);
            if (!mysql_real_connect(connection, getServerIp().c_str(), getAdmin().c_str(), getPassword().c_str(), getDatabase().c_str(), atoi(getPort().c_str()), NULL, 0)) {
                cout << "*** Not connected: " << mysql_error(connection) << endl;
                return 1;
            } else {
                conn = connection;
                cout << "*** Connected" << endl;
                return 0;
            }
            return 0;
        }

        int performQuery(string query) {
            if (mysql_query(conn, query.c_str())) {
                cout << query.c_str() << endl;
                cout << "error in performQuery() function: " << mysql_error(conn) << endl;
                return 1;
            }
            res = mysql_use_result(conn);
            return 0;
        }

        int disConnection() {
            mysql_close(conn);
            cout << "*** Disconnected." << endl;
            return 0;
        }

        int insert(string blockId, string userId, string varType, string varName, string varValue, string path) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 1) {
                query = "INSERT INTO ledger (block_id, user_id, var_type, var_name, var_value, merkle_path) VALUES('"+ blockId + "', '"+ userId + "', '" + varType + "', '" + varName + "', '" + varValue + "', '" + path + "')";
                if(performQuery(query) == 0) {
                    cout << "insert() function was processed!!!" << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << "insert() function was not processed!!!" << endl;
                    mysql_free_result(res);
                    return 1;
                }
            } else {
                cout << "insert() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            cout << "insert() function was processed!!!" << endl;
            mysql_free_result(res);
            return 0;
        }

        int updateVarValue(string userId, string varType, string varName, string varValue) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 0) {
                query = "UPDATE ledger SET var_value='" + varValue + "' WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName +"'";
                if(performQuery(query) == 0) {
                    cout << "updateVarValue() function was processed!!!" << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << "updateVarValue() function was not processed!!!" << endl;
                    mysql_free_result(res);
                    return 1;
                }
            } else {
                cout << "updateVarValue() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            cout << "updateVarValue() function was processed!!!" << endl;
            mysql_free_result(res);
            return 0;
        }

        int deleteData(string userId, string varType, string varName) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 0) {
                query = "DELETE FROM ledger WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName + "'";
                if(performQuery(query) == 0) {
                    cout << "deleteData() function was processed!!!" << endl;
                    return 0;
                } else {
                    cout << "deleteData() function was not processed!!!" << endl;
                    return 1;
                }
            } else {
                cout << "deleteData() function was not processed!!!" << endl;
                return 1;
            }
            cout << "deleteData() function was processed!!!" << endl;
            mysql_free_result(res);
            return 0;
        }

        int selectAllUsingUserId(string userId) {
            query = "SELECT * FROM ledger WHERE user_id='" + userId + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    for(i=0; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return 0;
        }

        pair<int, vector<string>> selectAllUsingUserIdVarTypeVarName(string userId, string varType, string varName) {
            query = "SELECT * FROM ledger WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName + "'";
            pair<int, vector<string>> all;
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    all.first = stoi(row[0]);
                    for(i=1; i<columns; i++) {
                        all.second.push_back(row[i]);
                        //printf("%15s\t", row[i]);
                    }
                    //printf("\n");
                }
            }
            mysql_free_result(res);
            return all;
        }

        int selectValueUsingUserIdVarName(string userId, string varName) {
            int result = -1;
            query = "SELECT var_value FROM ledger WHERE user_id='" + userId + "' AND var_name='" + varName + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    result = stoi(row[0]);
                }
            }
            mysql_free_result(res);
            return result;
        }

        vector<pair<int, vector<string>>> selectAll() {
            query = "SELECT * FROM ledger ORDER BY record_id";
            vector<pair<int, vector<string>>> all;
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    pair<int, vector<string>> record;
                    printf("%15s\t", row[0]);
                    record.first = stoi(row[0]);
                    record.second.clear();
                    for(i=1; i<columns; i++) {
                        record.second.push_back(row[i]);
                        printf("%15s\t", row[i]);
                    }
                    all.push_back(record);
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return all;
        }

        int checkUserId(string userId) {
            query = "SELECT user_id FROM ledger WHERE user_id='" + userId + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    cout << row[0] << endl;
                    cout << "exists." << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << userId << endl;
                    cout << "does not exist." << endl;
                    mysql_free_result(res);
                    return 1;
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int checkVarName(string varName) {
            query = "SELECT var_name FROM ledger WHERE var_name='" + varName + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    cout << row[0] << endl;
                    cout << "exists." << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << varName << endl;
                    cout << "does not exist." << endl;
                    mysql_free_result(res);
                    return 1;
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int checkUserIdVarName(string *userId, string *varName) {
            query = "SELECT user_id, var_name FROM ledger WHERE user_id='" + *userId + "' AND var_name='" + *varName + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    cout << row[0] << ", "<< row[1] << endl;
                    cout << "exists." << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << *userId << ", " << *varName << endl;
                    cout << "does not exist." << endl;
                    mysql_free_result(res);
                    return 1;
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int checkUserIdVarTypeVarName(string *userId, string *varType, string *varName) {
            query = "SELECT user_id, var_type, var_name FROM ledger WHERE user_id='" + *userId + "' AND var_type='" + *varType + "' AND var_name='" + *varName + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    cout << row[0] << ", "<< row[1] << ", " << row[2] << endl;
                    cout << "exists." << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << *userId << ", " << *varType << ", " << *varName << endl;
                    cout << "does not exist." << endl;
                    mysql_free_result(res);
                    return 1;
                }
            }
            mysql_free_result(res);
            return 0;
        }


        int transferMoney(string fromUserId, string fromVarType, string fromVarName, string toUserId, string toVarType, string toVarName, int value) {

            cout << fromUserId << " / " << fromVarName << " / " << toUserId << " / " << toVarName << " / " << value << endl;

            if(checkBalance(&fromUserId, &fromVarName, &value) == 0) {
                int fromBal;
                int toBal;

                query = "SELECT var_value FROM ledger WHERE user_id='" + fromUserId + "' AND var_name='" + fromVarName + "'";
                if(performQuery(query) == 0) {
                    columns = mysql_num_fields(res); // the number of field
                    if((row = mysql_fetch_row(res)) != NULL) {
                        fromBal = atoi(row[0]);
                    }
                }
                // cout << fromBal << endl;
                mysql_free_result(res);

                query = "SELECT var_value FROM ledger WHERE user_id='" + toUserId + "' AND var_name='" + toVarName + "'";
                if(performQuery(query) == 0) {
                    columns = mysql_num_fields(res); // the number of field
                    if((row = mysql_fetch_row(res)) != NULL) {
                        toBal = atoi(row[0]);
                    }
                }
                // cout << toBal << endl;
                mysql_free_result(res);

                updateVarValue(fromUserId, fromVarType, fromVarName, to_string(fromBal-value)); // user_id, var_name and var_value of Database
                updateVarValue(toUserId, toVarType, toVarName, to_string(toBal+value)); // user_id, var_name and var_value of Database

                mysql_free_result(res);
                return 0;
            } else {
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 1;
        }

        int checkBalance(string *userId, string *varName, int *value) {
            // can check the etc, btc and gru and so on whether it is enough or not
            query = "SELECT var_value FROM ledger WHERE user_id='" + *userId + "' AND var_name='" + *varName + "'";;
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    if((atoi(row[0]) - *value) < 0) {
                        cout << *userId <<"'s balance: " << row[0] << ", not enough" <<  endl;
                        mysql_free_result(res);
                        return 1;
                    } else {
                        cout << *userId <<"'s balance: " << row[0] << ", enough" <<  endl;
                        mysql_free_result(res);
                        return 0;
                    }
                }
            }
            mysql_free_result(res);
            return 1;
        }

        int layerInsert(string blockId, string scContents) {
            cout << "previous data" << endl;
            if(layerSelectAll() < 10) {
                query = "INSERT INTO layer (block_id, sc_contents) VALUES('" + blockId + "', '" + scContents + "')";
                if(performQuery(query) == 0) {
                    cout << "layerInsert() function was processed!!!" << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << "layerInsert() function was not processed!!!" << endl;
                    mysql_free_result(res);
                    return 1;
                }
                mysql_free_result(res);
                return 0;
            } else {
                cout << "layer is full, so can insert more" << endl;
            }
            return 0;
        }

        int layerDelete(string blockId, string scContents) {
            query = "DELETE FROM layer WHERE block_id='" + blockId + "' AND sc_contents='" + scContents + "'";
            if(performQuery(query) == 0) {
                cout << "layerDelete() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "layerDelete() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int layerSelectAll() {
            int numLayer = 0;
            query = "SELECT * FROM layer ORDER BY record_id";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    numLayer++;
                    printf("%15s\t", row[0]);
                    for(i=1; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            // cout << numLayer << endl;
            mysql_free_result(res);
            return numLayer;
        }

        int blockInsert(string blockId, string blockHash) {
            query = "INSERT INTO block (block_id, block_hash) VALUES('" + blockId + "', '" + blockHash + "')";
            if(performQuery(query) == 0) {
                cout << "blockInsert() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "blockInsert() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int blockUpdateBlockHash(string blockId, string blockHash) {
            query = "UPDATE block SET block_hash='" + blockHash + "' WHERE block_id='" + blockId +"'";
            if(performQuery(query) == 0) {
                cout << "blockUpdateBlockHash() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "blockUpdateBlockHash() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int blockDelete(string blockId) {
            query = "DELETE FROM block WHERE block_id='" + blockId + "'";
            if(performQuery(query) == 0) {
                cout << "blockDelete() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "blockDelete() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int blockSelectUsingBlockId(string blockId) {
            query = "SELECT * FROM block WHERE block_id='" + blockId + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    for(i=0; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int blockSelectAll() {
            query = "SELECT * FROM block ORDER BY block_id";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    printf("%15s\t", row[0]);
                    for(i=1; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int transactionInsert(string blockId, string reqrst, string scContents) {
            query = "INSERT INTO transaction (block_id, req_rst, sc_contents) VALUES('" + blockId + "', '" + reqrst + "', '" + scContents + "')";
            if(performQuery(query) == 0) {
                cout << "transactionInsert() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "transactionInsert() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int transactionDelete(string blockId, string reqrst, string scContents) {
            query = "DELETE FROM transaction WHERE block_id='" + blockId + "' AND req_rst='" + reqrst + "' AND sc_contents='" + scContents + "'";
            if(performQuery(query) == 0) {
                cout << "transactionDelete() function was processed!!!" << endl;
                mysql_free_result(res);
                return 0;
            } else {
                cout << "transactionDelete() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 0;
        }

        int transactionSelectUsingBlockId(string blockId) {
            query = "SELECT * FROM transaction WHERE block_id='" + blockId + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    for(i=0; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return 0;
        }

        int transactionSelectAll() {
            query = "SELECT * FROM transaction ORDER BY block_id";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    printf("%15s\t", row[0]);
                    for(i=1; i<columns; i++) {
                        printf("%15s\t", row[i]);
                    }
                    printf("\n");
                }
            }
            mysql_free_result(res);
            return 0;
        }

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
