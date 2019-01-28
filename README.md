# Gruut storage

### Prerequisite
  - CMake (**3.12**)
  - nlohmann
    * [link](https://github.com/nlohmann/json)
  - SHA256
    * [link](http://www.zedwood.com/article/cpp-sha256-function)


### Example
  - **Test blocks**

        {
          "blocks": [
            {
              "transactions": [
                {"command": "add", "to_user_id": "mizno", "to_var_type": "coin", "to_var_name": "gru", "value": 1000},
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": -1000},
                {"command": "add", "to_user_id": "mizno", "to_var_type": "coin", "to_var_name": "gru", "value": 1000}
              ]
            },

            {
              "transactions": [
                {"command": "add", "to_user_id": "mizno", "to_var_type": "coin", "to_var_name": "gru", "value": 2000},
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": 2000},
                {"command": "new", "user_id": "kjh", "var_type": "string", "var_name": "user_name", "var_value": "jeonghyeon"},
                {"command": "new", "user_id": "kjh", "var_type": "coin", "var_name": "btc", "var_value": "10"},
                {"command": "new", "user_id": "kjh", "var_type": "coin", "var_name": "gru", "var_value": "100000"}
              ]
            },

            {
              "transactions": [
                {"command": "add", "to_user_id": "mizno", "to_var_type": "coin", "to_var_name": "gru", "value": 3000},
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": 3000},
                {"command": "del", "user_id": "kjh", "var_type": "coin", "var_name": "btc"},
                {"command": "add", "to_user_id": "kjh", "to_var_type": "coin", "to_var_name": "btc", "value": 10}
              ]
            },

            {
              "transactions": [
                {"command": "send", "from_user_id": "mizno", "from_var_type": "coin", "from_var_name": "gru", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": 5000},
                {"command": "send", "from_user_id": "mizno", "from_var_type": "coin", "from_var_name": "gru", "to_user_id": "kjh", "to_var_type": "coin", "to_var_name": "gru", "value": 1000}
              ]
            },

            {
              "transactions": [
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": -3000},
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": 1000}
              ]
            },

            {
              "transactions": [
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": -3000},
                {"command": "add", "to_user_id": "mang", "to_var_type": "coin", "to_var_name": "gru", "value": 1000}
              ]
            }
          ]
        }
  - **Test**

        gruut::Storage g_storage;

        // parse blocks from above test blocks
        g_storage.setBlocksByJson();

        // test storage
        g_storage.testStorage();

  - **Output 1 (Merkle tree setup with DB data)**

            1	          mizno	         string	      user_name	       changhun	          36545
            2	          mizno	           coin	            eth	            500	          52865
            3	          mizno	           coin	            btc	            100	          28222
            4	          mizno	           coin	            gru	         160000	          37876
            5	           mang	         string	      user_name	        daehong	          14958
            6	           mang	           coin	            eth	            300	          12527
            7	           mang	           coin	            btc	      123456789	          20203
            8	           mang	           coin	            gru	             50	          12487
           10	            kjh	           coin	            gru	         100000	          24189
           11	            kjh	         string	      user_name	     jeonghyeon	             87
        *********** print tree data by using post-order traversal... ************

        node size: 10

        print isPrint: 1
        [depth   0] Left
          [depth   1] Left
            [depth   2] Left
              [depth   3] Left
        			Left	[path: 0000000001010111] suffix_len: 13, suffix: 0000000001010111,  hash_value: 95298b40e581bbd16610827215c47e4ebd9a71ca659b31c2b871621c46f5eb
              [depth   3] Right
                [depth   4] Right
                  [depth   5] Left
                    [depth   6] Left
                      [depth   7] Left
                        [depth   8] Left
                          [depth   9] Right
                            [depth  10] Right
                              [depth  11] Left
        											Left	[path: 0011000011000111] suffix_len: 5, suffix: 0000000000000111,  hash_value: f9db1263407755119da74696f498ff6a15ced3b9f7efb2d951cbf5ebe8fd8398
                              [depth  11] Right
        											Right	[path: 0011000011101111] suffix_len: 5, suffix: 0000000000001111,  hash_value: 26f1b6769e935ba9c38ddb33365653ec45a7761ba8d3e1713f4c8019911fb94b
                  [depth   5] Right
        					Right	[path: 0011101001101110] suffix_len: 11, suffix: 0000001001101110,  hash_value: 87ebfc46599583da7615a9ca2aed90e1361c549eba25d387cfcb40d7e18a87
            [depth   2] Right
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 0100111011101011] suffix_len: 12, suffix: 0000111011101011,  hash_value: 95b3aabd60f755156ec18f3711e194868e1a84db2e1c1e0f3b2aa875ba7f636
                [depth   4] Right
        				Right	[path: 0101111001111101] suffix_len: 12, suffix: 0000111001111101,  hash_value: f06e2fc929f4aae04bf1f5815b4f2023c35ff91ed61a644786e9e984fcc39
              [depth   3] Right
        			Right	[path: 0110111000111110] suffix_len: 13, suffix: 0000111000111110,  hash_value: b8c235dff91b6a30abff5017d5bfe1d0244585d24e7480a052ca24e31219ef4f
          [depth   1] Right
            [depth   2] Left
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 1000111011000001] suffix_len: 12, suffix: 0000111011000001,  hash_value: a13a5d05c741562e135dbc6713db3539a87baf6e8e7b778056de7ff694be
                [depth   4] Right
        				Right	[path: 1001001111110100] suffix_len: 12, suffix: 0000001111110100,  hash_value: 6cffd1a8cf2156dff269e23c6b96ff3f3c958aaa0a56f6ca01aeba2feb348ca
            [depth   2] Right
        		Right	[path: 1100111010000001] suffix_len: 14, suffix: 0000111010000001,  hash_value: 3b47c2fbea4fede85a3ecdda6e18bf2d3827bd0b111c2bb294925e34a
        root Value: 23b8a3aab025cae0efef1b225081d52819397ed952be75d3a3b155954aee2f
        *********** finish traversal ***********


  - **Output 2 (printing parsed blocks from above test blocks)**

        Block #0
        	Transaction #0	{"command":"add","to_user_id":"mizno","to_var_name":"gru","to_var_type":"coin","value":1000}
        	Transaction #1	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":-1000}
        	Transaction #2	{"command":"add","to_user_id":"mizno","to_var_name":"gru","to_var_type":"coin","value":1000}
        Block #1
        	Transaction #0	{"command":"add","to_user_id":"mizno","to_var_name":"gru","to_var_type":"coin","value":2000}
        	Transaction #1	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":2000}
        	Transaction #2	{"command":"new","user_id":"kjh","var_name":"user_name","var_type":"string","var_value":"jeonghyeon"}
        	Transaction #3	{"command":"new","user_id":"kjh","var_name":"btc","var_type":"coin","var_value":"10"}
        	Transaction #4	{"command":"new","user_id":"kjh","var_name":"gru","var_type":"coin","var_value":"100000"}
        Block #2
        	Transaction #0	{"command":"add","to_user_id":"mizno","to_var_name":"gru","to_var_type":"coin","value":3000}
        	Transaction #1	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":3000}
        	Transaction #2	{"command":"del","user_id":"kjh","var_name":"btc","var_type":"coin"}
        	Transaction #3	{"command":"add","to_user_id":"kjh","to_var_name":"btc","to_var_type":"coin","value":10}
        Block #3
        	Transaction #0	{"command":"send","from_user_id":"mizno","from_var_name":"gru","from_var_type":"coin","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":5000}
        	Transaction #1	{"command":"send","from_user_id":"mizno","from_var_name":"gru","from_var_type":"coin","to_user_id":"kjh","to_var_name":"gru","to_var_type":"coin","value":1000}
        Block #4
        	Transaction #0	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":-3000}
        	Transaction #1	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":1000}
        Block #5
        	Transaction #0	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":-3000}
        	Transaction #1	{"command":"add","to_user_id":"mang","to_var_name":"gru","to_var_type":"coin","value":1000}

  - **Output 3 (Block #0, Block #1, Rollback)**

        *********** print tree data by using post-order traversal... ************

        node size: 10

        print isPrint: 1
        [depth   0] Left
          [depth   1] Left
            [depth   2] Left
              [depth   3] Left
        			Left	[path: 0000000001010111] suffix_len: 13, suffix: 0000000001010111,  hash_value: 95298b40e581bbd16610827215c47e4ebd9a71ca659b31c2b871621c46f5eb
              [depth   3] Right
                [depth   4] Right
                  [depth   5] Left
                    [depth   6] Left
                      [depth   7] Left
                        [depth   8] Left
                          [depth   9] Right
                            [depth  10] Right
                              [depth  11] Left
        											Left	[path: 0011000011000111] suffix_len: 5, suffix: 0000000000000111,  hash_value: f9db1263407755119da74696f498ff6a15ced3b9f7efb2d951cbf5ebe8fd8398
                              [depth  11] Right
        											Right	[path: 0011000011101111] suffix_len: 5, suffix: 0000000000001111,  hash_value: 26f1b6769e935ba9c38ddb33365653ec45a7761ba8d3e1713f4c8019911fb94b
                  [depth   5] Right
        					Right	[path: 0011101001101110] suffix_len: 11, suffix: 0000001001101110,  hash_value: 87ebfc46599583da7615a9ca2aed90e1361c549eba25d387cfcb40d7e18a87
            [depth   2] Right
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 0100111011101011] suffix_len: 12, suffix: 0000111011101011,  hash_value: 95b3aabd60f755156ec18f3711e194868e1a84db2e1c1e0f3b2aa875ba7f636
                [depth   4] Right
        				Right	[path: 0101111001111101] suffix_len: 12, suffix: 0000111001111101,  hash_value: f06e2fc929f4aae04bf1f5815b4f2023c35ff91ed61a644786e9e984fcc39
              [depth   3] Right
        			Right	[path: 0110111000111110] suffix_len: 13, suffix: 0000111000111110,  hash_value: b8c235dff91b6a30abff5017d5bfe1d0244585d24e7480a052ca24e31219ef4f
          [depth   1] Right
            [depth   2] Left
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 1000111011000001] suffix_len: 12, suffix: 0000111011000001,  hash_value: a13a5d05c741562e135dbc6713db3539a87baf6e8e7b778056de7ff694be
                [depth   4] Right
        				Right	[path: 1001001111110100] suffix_len: 12, suffix: 0000001111110100,  hash_value: 58359629724760b11413c9be42fc1e8e8cc29e2b278a1cdb70de779361e0
            [depth   2] Right
        		Right	[path: 1100111010000001] suffix_len: 14, suffix: 0000111010000001,  hash_value: 3b47c2fbea4fede85a3ecdda6e18bf2d3827bd0b111c2bb294925e34a
        root Value: 43192c2c2283339767c0611d7e7e581ce07e54aaded6ccabbc77333ca96bdd5
        *********** finish traversal ***********



        *********** print tree data by using post-order traversal... ************

        node size: 11

        print isPrint: 1
        [depth   0] Left
          [depth   1] Left
            [depth   2] Left
              [depth   3] Left
                [depth   4] Left
                  [depth   5] Left
        					Left	[path: 0000000001010111] suffix_len: 11, suffix: 0000000001010111,  hash_value: 95298b40e581bbd16610827215c47e4ebd9a71ca659b31c2b871621c46f5eb
                  [depth   5] Right
        					Right	[path: 0000101000010000] suffix_len: 11, suffix: 0000001000010000,  hash_value: d9c5cccac826493263ce06aa6d71e494bea2161d0fa4c48c07815127a4170eb
              [depth   3] Right
                [depth   4] Right
                  [depth   5] Left
                    [depth   6] Left
                      [depth   7] Left
                        [depth   8] Left
                          [depth   9] Right
                            [depth  10] Right
                              [depth  11] Left
        											Left	[path: 0011000011000111] suffix_len: 5, suffix: 0000000000000111,  hash_value: 90796cbf2fb5d7793a28f494eeaae78b382899cc6d92cdcf1296474c5e95b
                              [depth  11] Right
        											Right	[path: 0011000011101111] suffix_len: 5, suffix: 0000000000001111,  hash_value: 26f1b6769e935ba9c38ddb33365653ec45a7761ba8d3e1713f4c8019911fb94b
                  [depth   5] Right
        					Right	[path: 0011101001101110] suffix_len: 11, suffix: 0000001001101110,  hash_value: 87ebfc46599583da7615a9ca2aed90e1361c549eba25d387cfcb40d7e18a87
            [depth   2] Right
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 0100111011101011] suffix_len: 12, suffix: 0000111011101011,  hash_value: 95b3aabd60f755156ec18f3711e194868e1a84db2e1c1e0f3b2aa875ba7f636
                [depth   4] Right
        				Right	[path: 0101111001111101] suffix_len: 12, suffix: 0000111001111101,  hash_value: f06e2fc929f4aae04bf1f5815b4f2023c35ff91ed61a644786e9e984fcc39
              [depth   3] Right
        			Right	[path: 0110111000111110] suffix_len: 13, suffix: 0000111000111110,  hash_value: b8c235dff91b6a30abff5017d5bfe1d0244585d24e7480a052ca24e31219ef4f
          [depth   1] Right
            [depth   2] Left
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 1000111011000001] suffix_len: 12, suffix: 0000111011000001,  hash_value: a13a5d05c741562e135dbc6713db3539a87baf6e8e7b778056de7ff694be
                [depth   4] Right
        				Right	[path: 1001001111110100] suffix_len: 12, suffix: 0000001111110100,  hash_value: 06ba93363d17a22519f4c75c5a6d35c41d1bd75afb172dd1591cec57349
            [depth   2] Right
        		Right	[path: 1100111010000001] suffix_len: 14, suffix: 0000111010000001,  hash_value: 3b47c2fbea4fede85a3ecdda6e18bf2d3827bd0b111c2bb294925e34a
        root Value: 3f80502b42d6331ced70185afd13dc2fb038d310bb478fc6bb3171ca23b97b
        *********** finish traversal ***********



        *********** print tree data by using post-order traversal... ************

        node size: 10

        print isPrint: 1
        [depth   0] Left
          [depth   1] Left
            [depth   2] Left
              [depth   3] Left
        			Left	[path: 0000000001010111] suffix_len: 13, suffix: 0000000001010111,  hash_value: 95298b40e581bbd16610827215c47e4ebd9a71ca659b31c2b871621c46f5eb
              [depth   3] Right
                [depth   4] Right
                  [depth   5] Left
                    [depth   6] Left
                      [depth   7] Left
                        [depth   8] Left
                          [depth   9] Right
                            [depth  10] Right
                              [depth  11] Left
        											Left	[path: 0011000011000111] suffix_len: 5, suffix: 0000000000000111,  hash_value: f9db1263407755119da74696f498ff6a15ced3b9f7efb2d951cbf5ebe8fd8398
                              [depth  11] Right
        											Right	[path: 0011000011101111] suffix_len: 5, suffix: 0000000000001111,  hash_value: 26f1b6769e935ba9c38ddb33365653ec45a7761ba8d3e1713f4c8019911fb94b
                  [depth   5] Right
        					Right	[path: 0011101001101110] suffix_len: 11, suffix: 0000001001101110,  hash_value: 87ebfc46599583da7615a9ca2aed90e1361c549eba25d387cfcb40d7e18a87
            [depth   2] Right
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 0100111011101011] suffix_len: 12, suffix: 0000111011101011,  hash_value: 95b3aabd60f755156ec18f3711e194868e1a84db2e1c1e0f3b2aa875ba7f636
                [depth   4] Right
        				Right	[path: 0101111001111101] suffix_len: 12, suffix: 0000111001111101,  hash_value: f06e2fc929f4aae04bf1f5815b4f2023c35ff91ed61a644786e9e984fcc39
              [depth   3] Right
        			Right	[path: 0110111000111110] suffix_len: 13, suffix: 0000111000111110,  hash_value: b8c235dff91b6a30abff5017d5bfe1d0244585d24e7480a052ca24e31219ef4f
          [depth   1] Right
            [depth   2] Left
              [depth   3] Left
                [depth   4] Left
        				Left	[path: 1000111011000001] suffix_len: 12, suffix: 0000111011000001,  hash_value: a13a5d05c741562e135dbc6713db3539a87baf6e8e7b778056de7ff694be
                [depth   4] Right
        				Right	[path: 1001001111110100] suffix_len: 12, suffix: 0000001111110100,  hash_value: 58359629724760b11413c9be42fc1e8e8cc29e2b278a1cdb70de779361e0
            [depth   2] Right
        		Right	[path: 1100111010000001] suffix_len: 14, suffix: 0000111010000001,  hash_value: 3b47c2fbea4fede85a3ecdda6e18bf2d3827bd0b111c2bb294925e34a
        root Value: 43192c2c2283339767c0611d7e7e581ce07e54aaded6ccabbc77333ca96bdd5
        *********** finish traversal ***********

    Root Value is same between Block #0 and after Rollback