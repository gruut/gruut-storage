# Gruut storage

### Prerequisite
  - CMake (**3.12**)
  - nlohmann
    * [link](https://github.com/nlohmann/json)
  - SHA256
    * [link](http://www.zedwood.com/article/cpp-sha256-function)


### Example
  - **Test data**

        struct test_data data[6] = {
        		{0, "a", "gru", "coin", 50},
        		{1, "b", "gru", "coin", 50},
        		{2, "c", "gru", "coin", 50},
        		{3, "d", "gru", "coin", 50},
        		{4, "e", "gru", "coin", 50},
        		{5, "f", "gru", "coin", 50}
        };

        uint path[6] = {
        		0,  // 000
        		2,  // 010
        		3,  // 011
        		4,  // 100
        		5,  // 101
        		6   // 110
        };
  - **Add Node**

        #include "merkle_lib.hpp"

        gruut::MerkleTree tree;
        tree.addNode(path[0], data[0]);
        tree.addNode(path[1], data[1]);
        tree.addNode(path[2], data[2]);
        tree.addNode(path[3], data[3]);

        // print for debugging
        tree.printTreePostOrder();

  - **Modify Node**

        #include "merkle_lib.hpp"

        gruut::MerkleTree tree;
        tree.addNode(path[0], data[0]);
        tree.addNode(path[1], data[1]);
        tree.printTreePostOrder();

        tree.modifyNode(path[0], data[2]);
        tree.printTreePostOrder();

  - **Remove Node**

        #include "merkle_lib.hpp"

        gruut::MerkleTree tree;
        tree.addNode(path[0], data[0]);
        tree.addNode(path[1], data[1]);
        tree.printTreePostOrder();

        tree.removeNode(path[1]);
        tree.printTreePostOrder();