#include "Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;



class BtreeNode{
        int n;
        bool leaf;
        int* keys;
        BtreeNode **child;
        Record *arr; 
    public:
        BtreeNode(int size){
        this->size = size;
        this->max_records = (size-sizeof(size)-sizeof(arr))/sizeof(Record);
        this->arr =  (Record*)malloc(max_records);
        };
        
        //READ
        //INSERT
        //DELETE

        

};