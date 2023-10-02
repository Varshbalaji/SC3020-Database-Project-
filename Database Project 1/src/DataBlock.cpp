#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;


// struct key_info{
//     value key 
//     vector <RecordRow> listOfrecordswithssamekey
// }

class DataBlock{
        int max_records;
        Record *arr; 
        DataBlock *nextBlk;
    public:
        DataBlock(size_t blockSize){
            this->max_records = (blockSize- sizeof(nextBlk)-sizeof(arr))/sizeof(Record);
            this->arr =  (Record*) malloc(max_records);
            this->nextBlk = nullptr;
        };
        
        //READ
        //INSERT
        //DELETE

        

};