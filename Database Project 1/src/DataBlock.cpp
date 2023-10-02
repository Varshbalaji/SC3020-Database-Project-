#include "../include/DataBlock.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;
#define NULL nullptr


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

        // READ Operation: Get a list of pointers to records based on one key
        vector<Record*> readRecordsByKey(short key) { // function will return a vector (a dynamic array-like container) of pointers to objects of type Record.
            vector<Record*> result; //declared a vector called result. This vector will store pointers 
            for (int i = 0; i < max_records; ++i) {
                if (arr[i].teamID == key) {
                    result.push_back(&arr[i]);
            }
        }
            return result;
    }

        //INSERT
        int insert(Record r){

            DataBlock* currentBlock = this; // Start from the current block
            int i = 0;
            for (i = 0; i < currentBlock->getMaxRecords(); ++i) {
                    Record temp = currentBlock->getRecord(i);
                    if(temp.teamID==-1)
                    break;   
                }

            if(currentBlock->getRecord(i).teamID==-1)
            {
                arr[i] = r;
                return i;
            }
            else
            {
                return -1;
            }
            
        }

    private:

        // Function to get the maximum number of records in the block
        int getMaxRecords() const {
            return max_records;
        }


        // Function to get a specific record by index
        Record getRecord(int index) {
            if (index >= 0 && index < max_records) {
                return arr[index];
            } else {
                Record r;
                r.teamID = -1;
            }
        }


        

};