
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
    this->StoragePtr = new char[storageSize]; // Create StoragePool as Char[] array of Storage  size
    this ->blockPtr = nullptr; //ptr to the beginning of the block 
    this -> MaxAddress = StoragePtr + StorageSize;
    this->blockRecordAddress = nullptr;

}

RecordAddress Storage::insertRecord(unsigned int recordSize, void *record){
    RecordAddress recordAddress;
    unsigned int splitRecordSize, spillOverRecordSize;

    if (recordSize > BlockSize){
        cout << "Fatal Error: Record Size can't be more than Block Size" ;
        throw invalid_argument("Record size Invalid!");
    }
 
    if (  (CurrentFreeBlockOffset < BlockSize) && (CurrentFreeBlockOffset + recordSize > BlockSize)  ) {  // Handle Spanned records across blocks

        // We need to span and fill in current block to the limit and increment current block to fill. 
        // Validate if are at limit with Storage and raise exeception
    
        if (( CurrentFreeBlockNumber + 1) > MaxNumberOfBlocks) {  
            cout << "Fatal Error: No more free space in memory!";
            throw invalid_argument("No More Free Space!");
        }
        recordAddress = {CurrentFreeBlockNumber, CurrentFreeBlockOffset};  // Save record address before spanning        
        splitRecordSize = BlockSize - CurrentFreeBlockOffset; // Fill splitRecordSize dataRecord in current block free space
        spillOverRecordSize = recordSize - splitRecordSize;   // Fill spillOverRecordSize  dataRecord in next block to span across

        blockRecordAddress = StoragePtr + ( (CurrentFreeBlockNumber - 1) * BlockSize) + CurrentFreeBlockOffset;
        memcpy(blockRecordAddress , record, splitRecordSize); // Copied to previous Block

        CurrentFreeBlockNumber ++;                                    // Go to next block
        CurrentFreeBlockOffset = 0;                                   // Start at 0 offset in next block

        blockRecordAddress = StoragePtr + ( (CurrentFreeBlockNumber - 1) * BlockSize) + CurrentFreeBlockOffset;
        memcpy((char *)blockRecordAddress , (record + splitRecordSize), spillOverRecordSize); // advance record pointer and fll rest of record in next block
        CurrentFreeBlockOffset += spillOverRecordSize;
    }
    else {
        if (CurrentFreeBlockOffset == BlockSize) {
                
            if (( CurrentFreeBlockNumber + 1) > MaxNumberOfBlocks) {  
                cout << "Fatal Error: No more free space in memory!";
                throw invalid_argument("No More Free Space!");
            }
            CurrentFreeBlockNumber ++;                                    // Go to next block
            CurrentFreeBlockOffset = 0;                                   // Start at 0 offset in next block
        }
        blockRecordAddress = StoragePtr + ( (CurrentFreeBlockNumber - 1) * BlockSize) + CurrentFreeBlockOffset;
        memcpy((char *)blockRecordAddress , record, recordSize);
        recordAddress = {CurrentFreeBlockNumber, CurrentFreeBlockOffset};
        CurrentFreeBlockOffset += recordSize;        
    }
    TotalNumberOfRecords ++;
    return recordAddress;
}

bool Storage::deleteRecord(RecordAddress recordAddress,unsigned int recordSize){
    try{
        // Get Address of the Record to be deleted within the block 
        char *dataRecord;
        dataRecord = StoragePtr + ( (recordAddress.blockNumber - 1) * BlockSize) + recordAddress.offset;
        if (recordAddress.blockNumber > MaxNumberOfBlocks){
            cout << "Error: Record to be deleted is beyond allocated memory ! Block " << recordAddress.blockNumber << " ; Offset " << recordAddress.offset << "\n";
            return false;
        }
        if (recordAddress.blockNumber > CurrentFreeBlockNumber ||
            (recordAddress.blockNumber == CurrentFreeBlockNumber && recordAddress.offset >= CurrentFreeBlockOffset)){ 
            cout << "Error: Trying to delete non-existent record at Block " << recordAddress.blockNumber << " ; Offset " << recordAddress.offset << "\n";
            return false;
        }

        if (dataRecord[0] == '\000'){ 
            cout << "Error: Trying to delete record that has been deleted !" << "\n";
            return false;
        }
        
        // 
        memset(dataRecord, '\0', recordSize); // Fill Empty space with nulls

        TotalNumberOfRecords --;
        return true;
    }
    catch(exception &e){
        cout << "Error: Deletion  of Record failed!" << "\n";
        cout << e.what() << "\n";
        return false;
    }
}

Record* Storage::getRecord(RecordAddress recordAddress, unsigned int recordSize){
    char *dataRecord;
    dataRecord  = StoragePtr + ((recordAddress.blockNumber -1) * BlockSize ) + recordAddress.offset;

    if (recordAddress.blockNumber > MaxNumberOfBlocks){
        cout << "Error: Record Address to be fetched is beyond allocated memory ! Block " << recordAddress.blockNumber << " ; Offset " << recordAddress.offset << "\n";
        return nullptr;
    }
    if (recordAddress.blockNumber > CurrentFreeBlockNumber ||
        (recordAddress.blockNumber == CurrentFreeBlockNumber && recordAddress.offset >= CurrentFreeBlockOffset)){ 
        cout << "Error: Trying to get non-existent record at Block " << recordAddress.blockNumber << " ; Offset " << recordAddress.offset << "\n";
        return nullptr;
    }

    if (dataRecord[0] == '\000'){ 
        return nullptr;
    }

    return ((Record *) dataRecord);
}

RecordAddress Storage::getNextRecordAddress(RecordAddress recordAddress, unsigned int recordSize){
    RecordAddress nextRecordAddress;
    bool exitNextRecordSearch = false; //flag for exiting the search for a next record (we search fort he next record beacuse of the possibility of deleted records in between)
    try{
        nextRecordAddress.blockNumber = recordAddress.blockNumber;
        nextRecordAddress.offset = recordAddress.offset;
        //cout << "in getNextRecordAddres " << nextRecordAddress.blockNumber << "**" << nextRecordAddress.offset;
        while (!exitNextRecordSearch){
            nextRecordAddress.offset  +=  recordSize; // next record's offset is current record offset plus recordSize
            if ( nextRecordAddress.offset  >= BlockSize){ // if the next record begins in the next block beacuse of deleted records in between 
                nextRecordAddress.offset  -= BlockSize; // offset of next record in next block 
                nextRecordAddress.blockNumber += 1;
            }
            if (nextRecordAddress.blockNumber > CurrentFreeBlockNumber ||
                (nextRecordAddress.blockNumber == CurrentFreeBlockNumber && nextRecordAddress.offset >= CurrentFreeBlockOffset)){ //possibility that that our block number and/or block offset is pointing to end of recordds
                exitNextRecordSearch = true;
                nextRecordAddress.blockNumber = -1;
                nextRecordAddress.offset = -1;
                return nextRecordAddress;
            }
            blockRecordAddress = StoragePtr + ((nextRecordAddress.blockNumber -1) * BlockSize ) + nextRecordAddress.offset ; // absolute address of next record
            char * startCheck = blockRecordAddress; //Check for if the next record address we derived is the address of a deleted record 
            if  (startCheck[0] != '\000'){
                exitNextRecordSearch = true;
            }
        }
       
    }
    catch(exception &e){
        cout << "Error: Fetching NextRecor d failed!" << "\n";
        cout << e.what() << "\n";
    }
    
    return nextRecordAddress;
}


Storage::~Storage()  // Destructor method
{
//    delete this->StoragePtr;  
    StoragePtr = nullptr;
}