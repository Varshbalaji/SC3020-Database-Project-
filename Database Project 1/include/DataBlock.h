#ifndef DATABLOCK_H
#define DATABLOCK_H
#include <tuple>
#include <vector>
using namespace std;

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
    private:
        int getMaxRecords();
        Record& getRecord(int index);
    public:
        int size;
        int max_records;
        Record *arr; 
  
        

};

#endif