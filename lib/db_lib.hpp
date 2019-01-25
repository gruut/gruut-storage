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
#include <json.hpp>

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

            pKindOfTransaction = js["kind of transaction"];
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

        int insert(string recordId, string userId, string varType, string varName, string varValue) {return 0;}
        int update(string userId, string varType, string varName, string varValue) {return 0;}
        int deleteData(string userId, string varType, string varName) {return 0;}
        int selectAllUsingUserId() {return 0;}
        int selectValueUsingUserIdVarName(string userId, string varName) { return 0; }
        int selectAll() {return 0;}

        int checkUserId(string userId) {return 0;}
        int checkVarName(string varName) {return 0;}
        int checkUserIdVarName(string userId, string varName) {return 0;}
        int checkUserIdVarTypeVarName(string *userId, string *varType, string *varName) {return 0;}

        // for money transfer
        int transferMoney(string fromUserId, string fromVarType, string fromVarName, string toUserId, string toVarType, string toVarName, int value)  {return 0;}
        int checkAccBal(string userId, string varName, string Value) {return 0;}

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

        int insert(string userId, string varType, string varName, string varValue, string path) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 1) {
                query = "INSERT INTO test (user_id, var_type, var_name, var_value, path) VALUES('"+ userId + "', '" + varType + "', '" + varName + "', '" + varValue + "', '" + path + "')";
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

        int update(string userId, string varType, string varName, string varValue) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 0) {
                query = "UPDATE test SET var_value='" + varValue + "' WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName +"'";
                if(performQuery(query) == 0) {
                    cout << "update() function was processed!!!" << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << "update() function was not processed!!!" << endl;
                    mysql_free_result(res);
                    return 1;
                }
            } else {
                cout << "update() function was not processed!!!" << endl;
                mysql_free_result(res);
                return 1;
            }
            cout << "update() function was processed!!!" << endl;
            mysql_free_result(res);
            return 0;
        }

        int deleteData(string userId, string varType, string varName) {
            if(checkUserIdVarTypeVarName(&userId, &varType, &varName) == 0) {
                query = "DELETE FROM test WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName + "'";
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
            query = "SELECT * FROM test WHERE user_id='" + userId + "'";
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
        pair< int, vector<string> > selectAllUsingUserIdVarTypeVarName(string userId, string varType, string varName) {
            query = "SELECT * FROM test WHERE user_id='" + userId + "' AND var_type='" + varType + "' AND var_name='" + varName + "'";
            pair< int, vector<string> > all;
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
            query = "SELECT var_value FROM test WHERE user_id='" + userId + "' AND var_name='" + varName + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    result = stoi(row[0]);
                }
            }
            mysql_free_result(res);
            return result;
        }

        vector< pair< int, vector<string> > > selectAll() {
            query = "SELECT * FROM test ORDER BY record_id";
            vector< pair< int, vector<string> > > all;
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    pair< int, vector<string> > record;
                    //printf("%15s\t", row[0]);
                    record.first = stoi(row[0]);
                    record.second.clear();
                    for(i=1; i<columns; i++) {
                        record.second.push_back(row[i]);
                        //printf("%15s\t", row[i]);
                    }
                    all.push_back(record);
                    //printf("\n");
                }
            }
            mysql_free_result(res);
            return all;
        }

        int checkUserId(string userId) {
            query = "SELECT user_id FROM test WHERE user_id='" + userId + "'";
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
            query = "SELECT var_name FROM test WHERE var_name='" + varName + "'";
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
            query = "SELECT user_id, var_name FROM test WHERE user_id='" + *userId + "' AND var_name='" + *varName + "'";
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
            query = "SELECT user_id, var_name FROM test WHERE user_id='" + *userId + "' AND var_type='" + *varType + "' AND var_name='" + *varName + "'";
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

            if(checkAccBal(&fromUserId, &fromVarName, &value) == 0) {
                int fromBal;
                int toBal;

                query = "SELECT var_value FROM test WHERE user_id='" + fromUserId + "' AND var_name='" + fromVarName + "'";
                if(performQuery(query) == 0) {
                    columns = mysql_num_fields(res); // the number of field
                    if((row = mysql_fetch_row(res)) != NULL) {
                        fromBal = atoi(row[0]);
                    }
                }
                // cout << fromBal << endl;
                mysql_free_result(res);

                query = "SELECT var_value FROM test WHERE user_id='" + toUserId + "' AND var_name='" + toVarName + "'";
                if(performQuery(query) == 0) {
                    columns = mysql_num_fields(res); // the number of field
                    if((row = mysql_fetch_row(res)) != NULL) {
                        toBal = atoi(row[0]);
                    }
                }
                // cout << toBal << endl;
                mysql_free_result(res);

                update(fromUserId, fromVarType, fromVarName, to_string(fromBal-value)); // user_id, var_name and var_value of Database
                update(toUserId, toVarType, toVarName, to_string(toBal+value)); // user_id, var_name and var_value of Database

                mysql_free_result(res);
                return 0;
            } else {
                mysql_free_result(res);
                return 1;
            }
            mysql_free_result(res);
            return 1;
        }

        int checkAccBal(string *userId, string *varName, int *value) {
            query = "SELECT var_value FROM test WHERE user_id='" + *userId + "' AND var_name='" + *varName + "'";;
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
