#include "Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;



class DataBlock{
        int max_records;
        Record *arr; 
    public:
        DataBlock(int size){
        this->max_records = (size - sizeof(arr))/sizeof(Record*);
        this->arr =  (Record*)malloc(sizeof(Record)*max_records);
        };
        
        //READ
        //INSERT
        //DELETE

        

};