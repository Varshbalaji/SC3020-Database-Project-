#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
#include <list>
#include <string>
using namespace std;

struct RecordAddress {
    int blockNumber;
    short  offset;
};

//Defining structure for a record 
struct Record{
    char gameDate[11];
    long teamID;
    int homePts;
    float FG_PCT_home;
    float FT_PCT_home;
    float FG3_PCT_home;
    int AST_home;
    int REB_home;
    int HOME_TEAM_WINS;

};


class Storage
{
    public:
        Storage(unsigned int StoragePoolSize, unsigned int BlockSize);
        virtual ~Storage();
        RecordAddress insertRecord(unsigned int recordSize, void *record);
        bool deleteRecord(RecordAddress address, unsigned int recordSize);
        tuple<Record*, int> getRecord(RecordAddress recordAddress, unsigned int recordSize); //get record identified by RecordAddres & Number of data blocks accessed

        RecordAddress getNextRecordAddress(RecordAddress address, unsigned int recordSize); //get next record following the one we need

        unsigned int getBlockSize(){
            return BlockSize;
        }

        unsigned int getStorageSize(){
            return StorageSize;
        }

        unsigned int getTotalNumberOfRecords(){
            return TotalNumberOfRecords;
        }
        unsigned int getMaxNumberOfBlocks(){
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
        char *blockPtr;

        unsigned int BlockSize; // size of each block
        unsigned int StorageSize; // max memory size of Storage  
        char *MaxAddress;
        int CurrentFreeBlockNumber;
        short CurrentFreeBlockOffset;
        unsigned int TotalNumberOfRecords;
        unsigned int MaxNumberOfBlocks;

};

#endif // STORAGE_H