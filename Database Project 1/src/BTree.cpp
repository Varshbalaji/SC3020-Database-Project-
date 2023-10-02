#include "Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "BplusTree.h"

using namespace std;

Btree::Btree(int n){
    this->pointers =(BTreeNode*)(malloc(n*sizeof(BTreeNode)));
    this->keys =(short*)(malloc(n*sizeof(short)));
};
