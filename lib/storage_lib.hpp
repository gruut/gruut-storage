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
        void select() {}
        void selectAll() {}

    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    class mariaDb: public dbSetting {

    private:

        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
        int fields;
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
            performQuery(query);
        }

        void update(string userId, string varName, string varValue) {
            query = "UPDATE test SET var_value='" + varValue + "' WHERE user_id='" + userId + "' AND var_name='" + varName +"'";
            performQuery(query);
        }

        void deleteData(string userId, string varName) {
            query = "DELETE FROM test WHERE user_id='" + userId + "' AND var_name='" + varName + "'";
            performQuery(query);
        }

        void selectUsingUserId(string userId) {

        }

        void selectAll() {
            query = "SELECT * FROM test ORDER BY record_id";
            if(performQuery(query) == 0) {
                fields = mysql_num_fields(res); // the number of field
                while((row = mysql_fetch_row(res)) != NULL) {
                    for(i=0; i<fields; i++) {
                        printf("%10s\t", row[i]);
                    }
                    printf("\n");
                }
            }
        }
    };

}

#endif //WORKSPACE_STORAGE_LIB_HPP
