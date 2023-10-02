#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
#include <list>
using namespace std;

struct RecordRow{
    void *blockAddress;
    short int offsetWithinBlock;
};

//Defining structure for a record 
struct Record{
    // RecordRow recordAddress;
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



class DataBlock{
    
    public:
    int size;
    int max_records;
    Record *arr; 
  
        

};



class Storage{
    public:
        Storage(size_t StorageSize, size_t blockSize);
        bool assignBlock();

    private :
        char *blockPtr;
        char *StoragePtr;
        size_t blockSize;
        size_t StorageSize; // max size of storage 
        size_t curNumberOFRecords; // number of records stored in storage
        size_t UsedBlocks; // number of of blocks used
        vector<DataBlock> blocklist;

        int assignedBlocks;
        int freeBlocks;


};


#endif 