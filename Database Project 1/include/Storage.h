#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
using namespace std;

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
    short HOME_TEAM_WINS;
};

class Block{
    short free_space_in_block_offset;  // two bytes to maintain offset in a block where free space is present 
    unsigned char *remaining_block_allocation = new unsigned char[398];  // remaining 398 bytes of block allocation
};

class Storage{
    private :
     
    public:
        Storage(size_t StorageSize, size_t blockSize);
        bool assignBlock();



};





#endif 