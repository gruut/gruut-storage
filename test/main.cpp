#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <merkle_lib.hpp>
using namespace std;


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
		3,	// 011
		4,  // 100
		5,  // 101
		6   // 110
};

#define _DATA_LEN 6

int main() {
    gruut::MerkleTree tree = gruut::MerkleTree();

//    for(int i = 0; i < _DATA_LEN; ++i) {
//        tree.addNode(path[i], data[i]);
//        cout << tree.getRootValue() << '\n';
//    }

    tree.addNode(path[0], data[0]);
//    tree.addNode(path[1], data[1]);
//    tree.addNode(path[2], data[2]);
//    tree.addNode(path[3], data[3]);

    tree.printTreePostOrder();
    tree.removeNode(path[0]);

    tree.printTreePostOrder();


    return 0;
}