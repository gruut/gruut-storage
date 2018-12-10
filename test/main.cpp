#include <stdio.h>
#include <stdlib.h>
#include <iostream>
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

    tree.addNode(path[3], data[3]);
    tree.printTreePostOrder();
	tree.addNode(path[4], data[4]);
	tree.printTreePostOrder();

    tree.removeNode(path[4]);
    tree.printTreePostOrder();

    return 0;
}