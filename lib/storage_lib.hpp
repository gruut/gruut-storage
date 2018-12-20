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

        void insert(string recordId, string userId, string varType, string varName, string varValue) {}
        void update(string userId, string varName, string varValue) {}
        void deleteData(string userId, string varName) {}
        void selectUsingUserId() {}
        void selectAll() {}

        int checkUserId(string userId) {return 0;}

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
                printf("MYSQL query error : %s\n", mysql_error(conn));
                exit(1);
            }
            res = mysql_use_result(conn);
            return 0;
        }

        int disConnection() {
            mysql_close(conn);
            cout << "*** Disconnected." << endl;
            return 0;
        }

        void insert(string recordId, string userId, string varType, string varName, string varValue) {
            query = "INSERT INTO test VALUES('"+ recordId +"', '" + userId + "', '" + varType + "', '" + varName + "', '" + varValue + "')";
            if(performQuery(query) == 0) {

            }
        }

        void update(string userId, string varName, string varValue) {
            if(checkUserId(userId) == 0) {
                query = "UPDATE test SET var_value='" + varValue + "' WHERE user_id='" + userId + "' AND var_name='" + varName +"'";
                if(performQuery(query) == 0) {

                }
            }
        }

        void deleteData(string userId, string varName) {
            query = "DELETE FROM test WHERE user_id='" + userId + "' AND var_name='" + varName + "'";
            if(performQuery(query) == 0) {

            }
        }

        void selectUsingUserId(string userId) {
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
        }

        void selectAll() {
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
        }

        int checkUserId(string userId) {
            query = "SELECT user_id FROM test WHERE user_id='" + userId + "'";
            if(performQuery(query) == 0) {
                columns = mysql_num_fields(res); // the number of field
                if((row = mysql_fetch_row(res)) != NULL) {
                    cout << row[0] << " user exists." << endl;
                    mysql_free_result(res);
                    return 0;
                } else {
                    cout << userId << " user does not exist." << endl;
                    mysql_free_result(res);
                    return 1;
                }
            }
            mysql_free_result(res);
            return 0;
        }
    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
