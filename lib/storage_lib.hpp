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

        string query;
        int m_amount;
        string m_from;
        string m_to;
        string m_type;
        int m_from_balance;
        int m_to_balance;
        int m_calculated_value;
        int m_problem; // check whether the process is ok(0) or not(1)

        struct connection_details {
            char const *server;
            char const *user;
            char const *password;
            char const *database;

        };

        MYSQL *mysqlConnectionSetup(struct connection_details mysql_details) {

            MYSQL *connection = mysql_init(NULL);

            if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password,
                                    mysql_details.database, 3307, NULL, 0)) {

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

        void dbInit() {
            connection_details mysqlD;
            mysqlD.server = "127.0.0.1";
            mysqlD.user = "root";
            mysqlD.password = "1234";
            mysqlD.database = "thevaulters";

            conn = mysqlConnectionSetup(mysqlD);

            if (conn != NULL) {
                cout << "*** You are connected to the database." << endl;
            } else {
                m_problem = 1;
            }
        }

        void parseJsonFromSmartContract(string json_data) {

            json js;

            // cout << "*** parse the json data" << endl;
            // cout << json_data << endl;

            js = json::parse(json_data);
            // cout << "js : " << js << endl;

            m_amount = js["amount"];
            m_from = js["from"];
            m_to = js["to"];
            m_type = js["type"];

            cout << "from smart contract: " << endl;
            cout << "amount: " << m_amount << " / from: " << m_from << " / to: " << m_to << " / type: " << m_type
                 << endl;

        }

        int getAmount() {
            return m_amount;
        }

        string getFrom() {
            return m_from;
        }

        string getTo() {
            return m_to;
        }

        string getType() {
            return m_type;
        }

        void checkUserId(string user_id) {
            query = "SELECT user_id FROM test WHERE user_id='" + user_id + "'";
            res = mysqlPerformQuery(conn, query.c_str());
            if ((row = mysql_fetch_row(res)) != NULL) {
                cout << row[0] << " user exists. " << endl;
            } else {
                cout << "user does not exist." << endl;
                m_problem = 1;
            }
            mysql_free_result(res);
        }

        void checkUserBalance(string user_id, string from_or_to) {
            query = "SELECT user_balance FROM test WHERE user_id='" + user_id + "'";
            res = mysqlPerformQuery(conn, query.c_str());
            if ((row = mysql_fetch_row(res)) != NULL) {
                cout << user_id << "'s balance: " << row[0] << endl;
            }

            // check whether from(sender)'s balance can be transfered
            if (from_or_to == "from") {
                // cout << "sender" << endl;
                m_from_balance = atoi(row[0]);
                if (!((m_from_balance - m_amount) < 0)) {
                    cout << user_id << "'s balance is enough" << endl;
                } else {
                    cout << user_id << "'s balance is not enough" << endl;
                    m_problem = 1;
                }
            } else if (from_or_to == "to") {
                // cout << "receiver" << endl;
                m_to_balance = atoi(row[0]);
            }
            mysql_free_result(res);
        }

        void updateUserBalance(string user_id, string from_or_to) {

            if (from_or_to == "from") {
                m_calculated_value = m_from_balance - m_amount;
                query = "UPDATE thevaulters.test SET user_balance='" + to_string(m_calculated_value) +
                        "' WHERE user_id='" + user_id + "'";
                // cout << query << endl;
            } else {
                m_calculated_value = m_to_balance + m_amount;
                query = "UPDATE thevaulters.test SET user_balance='" + to_string(m_calculated_value) +
                        "' WHERE user_id='" + user_id + "'";
                // cout << query << endl;
            }
            mysqlPerformQuery(conn, query.c_str());
        }

    };
}

#endif //WORKSPACE_STORAGE_LIB_HPP
