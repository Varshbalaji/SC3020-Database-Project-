#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
#include <list>
using namespace std;

struct RecordAddress {
    unsigned int blockNumber;
    short  offset;
};

//Defining structure for a record 
struct Record{
    int gameDate;
    short teamID;
    short homePts;
    short FG_PCT_home;
    short FT_PCT_home;
    short FG3_PCT_home;
    short AST_home;
    short REB_home;
    bool HOME_TEAM_WINS;

};


class Storage
{
    public:
        Storage(unsigned int StorageSize, unsigned int BlockSize);
        virtual ~Storage();
        RecordAddress insertRecord(unsigned int recordSize, void *Record);
        bool deleteRecord(RecordAddress address, unsigned int recordSize);
        RecordAddress getRecord();

        unsigned int getBlockSize(){
            return BlockSize;
        }

        unsigned int getStorageSize(){
            return StorageSize;
        }

        unsigned int getTotalNumberOfRecords(){
            return TotalNumberOfRecords;
        }
        unsigned int getMaxNumberOfBlockss(){
            return MaxNumberOfBlocks;
        }
        unsigned int getUsedBlocks(){
            if (CurrentFreeBlockOffset == 0) {
                return CurrentFreeBlockNumber - 1;
            }
            else {
                return CurrentFreeBlockNumber;
            }
        }
        
    private:
        char *StoragePtr;
        char *blockRecordAddress;

        unsigned int  BlockSize; // size of each block
        unsigned int StorageSize; // max memory size of Storage  
        unsigned int CurrentFreeBlockNumber; //Current Free Block 
        unsigned int  CurrentFreeBlockOffset; // Current free space within a Block 
        unsigned int  TotalNumberOfRecords; // Total number of records in the Storage 
        unsigned int  MaxNumberOfBlocks; // maximum number of blocks in the Storage 

};



#endif 