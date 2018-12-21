//
// Created by CH on 2018-11-22.
//

#ifndef WORKSPACE_STORAGE_LIB_HPP
#define WORKSPACE_STORAGE_LIB_HPP

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <iostream>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

namespace gruut {

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
        int update(string userId, string varName, string varValue) {return 0;}
        int deleteData(string userId, string varName) {return 0;}
        int selectAllUsingUserId() {return 0;}
        int selectAll() {return 0;}

        int checkUserId(string userId) {return 0;}
        int checkVarName(string varName) {return 0;}
        int checkUserIdVarName(string userId, string varName) {return 0;}

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

        int insert(string recordId, string userId, string varType, string varName, string varValue) {
            if(checkUserIdVarName(&userId, &varName) == 1) {
                query = "INSERT INTO test VALUES('"+ recordId +"', '" + userId + "', '" + varType + "', '" + varName + "', '" + varValue + "')";
                if(performQuery(query) == 0) {
                    cout << "insert() function was processed!!!" << endl;
                    return 0;
                } else {
                    cout << "insert() function was not processed!!!" << endl;
                    return 1;
                }
            } else {
                cout << "insert() function was not processed!!!" << endl;
                return 1;
            }
            cout << "insert() function was processed!!!" << endl;
            return 0;
        }

        int update(string userId, string varName, string varValue) {
            if(checkUserIdVarName(&userId, &varName) == 0) {
                query = "UPDATE test SET var_value='" + varValue + "' WHERE user_id='" + userId + "' AND var_name='" + varName +"'";
                if(performQuery(query) == 0) {
                    cout << "update() function was processed!!!" << endl;
                    return 0;
                } else {
                    cout << "update() function was not processed!!!" << endl;
                    return 1;
                }
            } else {
                cout << "update() function was not processed!!!" << endl;
                return 1;
            }
            cout << "update() function was processed!!!" << endl;
            return 0;
        }

        int deleteData(string userId, string varName) {
            if(checkUserIdVarName(&userId, &varName) == 0) {
                query = "DELETE FROM test WHERE user_id='" + userId + "' AND var_name='" + varName + "'";
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

        int selectAll() {
            query = "SELECT * FROM test ORDER BY record_id";
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
            query = "SELECT user_id, var_name FROM test WHERE user_id='" + *userId + "' AND var_name='" + *varName + "'";;
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

#endif //WORKSPACE_STORAGE_LIB_HPP
