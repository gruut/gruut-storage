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

        string server_ip;
        string admin;
        string password;
        string database;
        string port;
        int m_problem; // for debugging
        string query;

    public:

        dbSetting() {
            cout << "*** dbSetting() is called" << endl;
        }

        ~dbSetting() {
            cout << "*** ~dbSetting() is called" << endl;
        }

        void setServerIp(string param) {server_ip = param;}
        string getServerIp() {return server_ip;}

        void setAdmin(string param) {admin = param;}
        string getAdmin() {return admin;}

        void setPassword(string param) {password = param;}
        string getPassword() {return password;}

        void setDatabase(string param) {database = param;}
        string getDatabase() {return database;}

        void setPort(string param) {port = param;}
        string getPort() {return port;}

        int getProblem() {return m_problem;}

        void connectionSetup() {}
        void performQuery(string query) {}
        void disConnection() {}

        void funcInsert(string record_id, string user_id, string var_type, string var_name, string var_value) {}
        void funcUpdate(string user_id, string var_name, string var_value) {}
        void funcDelete(string user_id, string var_name) {}
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

        void connectionSetup() {
            MYSQL *connection = mysql_init(NULL);
            if (!mysql_real_connect(connection, getServerIp().c_str(), getAdmin().c_str(), getPassword().c_str(), getDatabase().c_str(), atoi(getPort().c_str()), NULL, 0)) {
                cout << "*** Not connected: " << mysql_error(connection) << endl;
                m_problem = 1;
            } else {
                conn = connection;
                cout << "*** Connected" << endl;
                m_problem = 0;
            }
        }

        void performQuery(string query) {
            if (mysql_query(conn, query.c_str())) {
                cout << query.c_str() << endl;
                printf("MYSQL query error : %s\n", mysql_error(conn));
                exit(1);
            }
            res = mysql_use_result(conn);
        }

        void disConnection() {
            mysql_close(conn);
            cout << "*** Disconnected." << endl;
        }

        void funcInsert(string record_id, string user_id, string var_type, string var_name, string var_value) {
            query = "INSERT INTO test VALUES('"+ record_id +"', '" + user_id + "', '" + var_type + "', '" + var_name + "', '" + var_value + "')";
            performQuery(query);
        }

        void funcUpdate(string user_id, string var_name, string var_value) {
            query = "UPDATE test SET var_value='" + var_value + "' WHERE user_id='" + user_id + "' AND var_name='" + var_name +"'";
            performQuery(query);
        }

        void funcDelete(string user_id, string var_name) {
            query = "DELETE FROM test WHERE user_id='" + user_id + "' AND var_name='" + var_name + "'";
            performQuery(query);
        }

        void selectAll() {
            query = "SELECT * FROM test ORDER BY record_id";
            performQuery(query);
            fields = mysql_num_fields(res); // the number of field
            while((row = mysql_fetch_row(res)) != NULL) {
                for(i=0; i<fields; i++) {
                    printf("%10s\t", row[i]);
                }
                printf("\n");
            }
        }
    };

}

#endif //WORKSPACE_STORAGE_LIB_HPP
