
#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <list>
using namespace std;


Storage::Storage(unsigned int storageSize,unsigned int blockSize)
{
    this->StorageSize = storageSize;
    this->BlockSize = blockSize;
    this->CurrentFreeBlockNumber = 1;
    this->MaxNumberOfBlocks = storageSize/blockSize;
    this->TotalNumberOfRecords = 0;
    this->CurrentFreeBlockOffset = 0;
    this->StoragePtr = new char[storageSize]; // Create StoragePool as Char[] array of Storage pool size
    this->blockRecordAddress = nullptr;

}

RecordAddress Storage::insertRecord(unsigned int recordSize, void *Record){
    if (recordSize > BlockSize){
        cout << " Error: Record Size can't be more than Block Size" ;
        throw invalid_argument("Record size Invalid!");
    }
 
    if (CurrentFreeBlockOffset + recordSize >= BlockSize) { // If current block spills over then advance to next block.. Right now we don't support Spanned records
        CurrentFreeBlockNumber += 1;
        CurrentFreeBlockOffset = 0;
    }

    if (CurrentFreeBlockNumber > MaxNumberOfBlocks) {
        cout << "Error: No more free space in memory!";
        throw invalid_argument("No More Free Space!");
    }
    // Address where we store the record 
    blockRecordAddress = StoragePtr + ( (CurrentFreeBlockNumber - 1) * BlockSize) + CurrentFreeBlockOffset;
    memcpy((char *)blockRecordAddress , Record, recordSize);
    RecordAddress recordAddress = {CurrentFreeBlockNumber, CurrentFreeBlockOffset};
    TotalNumberOfRecords ++;
    CurrentFreeBlockOffset += recordSize;

    return recordAddress;
}

bool Storage::deleteRecord(RecordAddress recordAddress,unsigned int recordSize){
    try{
        // Get Address of the Record to be deleted within the block 
        blockRecordAddress = StoragePtr + ( (recordAddress.blockNumber - 1) * BlockSize) + recordAddress.offset;
        
        // 
        memset((char *)blockRecordAddress, '\0', recordSize); // Fill Empty space with nulls

        TotalNumberOfRecords --;
        return true;
    }
    catch(exception &e){
        cout << "Error: Deletion  of Record failed!" << "\n";
        cout << e.what() << "\n";
        return false;
    }
}

RecordAddress Storage::getRecord(){

    
};

Storage::~Storage()  // Destructor method
{
    delete StoragePtr;  
    StoragePtr = nullptr;
}
