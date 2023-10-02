#include "../include/Storage.h"
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

        vector<Record*> readRecordsInRange(short key1, short key2) {
            vector<Record*> result;

            DataBlock* currentBlock = this; // Start from the current block

            while (currentBlock != nullptr) {
                // Iterate through records in the current block
                for (int i = 0; i < currentBlock->getMaxRecords(); ++i) {
                    short currentKey = currentBlock->getRecord(i).teamID;
                    if (currentKey >= key1 && currentKey <= key2) {
                        result.push_back(&(currentBlock->getRecord(i)));
                    }
                }

                // Move to the next block (if available)
                currentBlock = currentBlock->getNextBlock();
            }

            return result;
        }

        //INSERT
        //DELETE







    private:

        // Function to get the maximum number of records in the block
        int getMaxRecords() const {
            return max_records;
        }


        // Function to get a specific record by index
        Record& getRecord(int index) {
            if (index >= 0 && index < max_records) {
                return arr[index];
            } else {
                // return the first record in case of an error
                return arr[0];
            }
        }

        

};