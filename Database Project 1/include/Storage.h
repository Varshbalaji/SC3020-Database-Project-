#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
using namespace std;

//Defining structure for a record 


class Storage{
    private :
     
    public:
        Storage(size_t StorageSize, size_t blockSize);
        bool assignBlock();



};





#endif 