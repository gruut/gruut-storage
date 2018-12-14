#include <iostream>
#include <cstring>
#include <sstream>
#include <merkle_lib.hpp>
#include "sha256/sha256.h"
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
		0,  // [0] 000
		2,  // [1] 010
		3,	// [2] 011
		4,  // [3] 100
		5,  // [4] 101
		6   // [5] 110
};

#define _DATA_LEN 6


int main() {
    gruut::MerkleTree tree = gruut::MerkleTree();

    tree.addNode(path[3], data[3]);
    printf("Root Hash value: %s\n", tree.getRootValueStr().c_str());
    tree.printTreePostOrder();

	tree.addNode(path[4], data[4]);
    printf("Root Hash value: %s\n", tree.getRootValueStr().c_str());

    vector<uint8_t> node = tree.getMerkleNode(path[3])->getValue();
    vector< vector<uint8_t> > siblings = tree.getSiblings(path[3]);
    for(auto v: siblings) {
        node = getHash(valueToStr(node), valueToStr(v));
        //cout << valueToStr(node) << endl;
    }
    cout << "Calculated root hash value = " << valueToStr(node) << endl;
	tree.printTreePostOrder();

    tree.removeNode(path[4]);
    printf("Root Hash value: %s\n", tree.getRootValueStr().c_str());
    tree.printTreePostOrder();

    return 0;
}