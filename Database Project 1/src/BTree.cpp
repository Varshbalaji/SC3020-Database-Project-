#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "BplusTree.h"

using namespace std;

struct RecordArrayDuplicateKey{
    
    int key_value; 
    vector<RecordAddress> storage_array;

};

Btree::Btree(int n){
    this->root = (BTreeNode*)malloc(sizeof(BTreeNode*));
    this->degree = n;
};

//search
//Insert
//Delete
