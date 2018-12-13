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

    class StorageLib {

    private:

        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
        int fields;
        int i;

        string query; // query variable
        int m_problem; // check whether the process is ok(0) or not(1)

        struct connection_details {
            char const *server;
            char const *user;
            char const *password;
            char const *database;
            unsigned int port;

        };

        MYSQL *mysqlConnectionSetup(struct connection_details mysql_details) {
            MYSQL *connection = mysql_init(NULL);
            if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, mysql_details.port, NULL, 0)) {
                printf("Connection error : %s\n", mysql_error(connection));
                exit(1);
            }
            return connection;
        }

        MYSQL_RES *mysqlPerformQuery(MYSQL *connection, char const *sql_query) {
            if (mysql_query(connection, sql_query)) {
                printf("MYSQL query error : %s\n", mysql_error(connection));
                exit(1);
            }
            return mysql_use_result(connection);
        }

    public:

        int getProblem() {
            return m_problem;
        }

        void dbConnect() {
            connection_details mysqlD;
            mysqlD.server = "127.0.0.1";
            mysqlD.user = "root";
            mysqlD.password = "1234";
            mysqlD.database = "thevaulters";
            mysqlD.port = 3307;

            conn = mysqlConnectionSetup(mysqlD);

            if (conn != NULL) {
                cout << "*** You are connected to the database." << endl;
            } else {
                m_problem = 1;
            }
        }

        void funcInsert(string record_id, string user_id, string var_type, string var_name, string var_value) {
            query = "INSERT INTO test VALUES('"+ record_id +"', '" + user_id + "', '" + var_type + "', '" + var_name + "', '" + var_value + "')";
            mysqlPerformQuery(conn, query.c_str());
        }

        void funcUpdate(string user_id, string var_name, string var_value) {
            query = "UPDATE test SET var_value='" + var_value + "' WHERE user_id='" + user_id + "' AND var_name='" + var_name +"'";
            mysqlPerformQuery(conn, query.c_str());
        }

        void funcDelete(string user_id, string var_name) {
            query = "DELETE FROM test WHERE user_id='" + user_id + "' AND var_name='" + var_name + "'";
            mysqlPerformQuery(conn, query.c_str());
        }

        void selectAll() {
            res = mysqlPerformQuery(conn, "SELECT * FROM test ORDER BY record_id");
            fields = mysql_num_fields(res); // the number of field

            while((row = mysql_fetch_row(res)) != NULL) {
                for(i=0; i<fields; i++) {
                    printf("%15s\t", row[i]);
                }
                printf("\n");
            }
        }

    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
