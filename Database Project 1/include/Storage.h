#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
using namespace std;

//Defining structure for a record 
struct RecordRow{
    void *blockAddress;
    short int offsetWithinBlock;
};
struct Record{
    RecordRow recordAddress;
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
    private :
     
    public:
        Storage(size_t StorageSize, size_t blockSize);
        bool assignBlock();



};





#endif 