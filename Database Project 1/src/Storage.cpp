
#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <list>
using namespace std;

Storage :: Storage(size_t StorageSize,size_t blockSize)
{
    this->StorageSize = StorageSize;
    this->blockSize = blockSize;
    this  -> freeBlocks = StorageSize/blockSize;
    //create storage    
    this -> blocklist = vector<DataBlock>();;
    this->StoragePtr = new char[StorageSize];
};

bool Storage::assignBlock(){
    if (freeBlocks > 0){


    }
}