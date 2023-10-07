#ifndef STORAGE_H
#define STORAGE_H
#include <tuple>
#include <vector>
#include <list>
#include <string>
using namespace std;


/**
 * @brief Structure to represent the address of a record.
 */

struct RecordAddress {
    int blockNumber;
    short  offset;
};

/**
 * @brief Structure to define the format of a record.
 */

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


/**
 * @brief Class for managing storage of records.
 */

class Storage
{
    public:
        /**
     * @brief Constructor for the Storage class.
     * @param StoragePoolSize The total size of the storage pool.
     * @param BlockSize The size of each storage block.
     */
        Storage(unsigned int StoragePoolSize, unsigned int BlockSize);
        
    /**
     * @brief Destructor for the Storage class.
     */

        virtual ~Storage();

    /**
     * @brief Inserts a record into the storage.
     * @param recordSize The size of the record to be inserted.
     * @param record A pointer to the record data.
     * @return The address of the inserted record.
     */
        RecordAddress insertRecord(unsigned int recordSize, void *record);

    /**
     * @brief Deletes a record from the storage.
     * @param address The address of the record to be deleted.
     * @param recordSize The size of the record to be deleted.
     * @return True if the record was successfully deleted, false otherwise.
     */
        bool deleteRecord(RecordAddress address, unsigned int recordSize);

    
    /**
     * @brief Retrieves a record from the storage.
     * @param recordAddress The address of the record to be retrieved.
     * @param recordSize The size of the record to be retrieved.
     * @return A tuple containing a pointer to the record and the number of data blocks accessed.
     */
        tuple<Record*, int> getRecord(RecordAddress recordAddress, unsigned int recordSize); //get record identified by RecordAddres & Number of data blocks accessed
    /**
     * @brief Gets the address of the next record following the specified one.
     * @param address The address of the current record.
     * @param recordSize The size of the records.
     * @return The address of the next record.
     */

        RecordAddress getNextRecordAddress(RecordAddress address, unsigned int recordSize); //get next record following the one we need

    /**
     * @brief Gets the size of each storage block.
     * @return The size of each storage block.
     */
        unsigned int getBlockSize(){
            return BlockSize;
        }
    /**
     * @brief Gets the total size of the storage.
     * @return The total size of the storage.
     */
        unsigned int getStorageSize(){
            return StorageSize;
        }
    /**
     * @brief Gets the total number of records stored.
     * @return The total number of records stored.
     */

        unsigned int getTotalNumberOfRecords(){
            return TotalNumberOfRecords;
        }

    /**
     * @brief Gets the maximum number of storage blocks.
     * @return The maximum number of storage blocks.
     */
        unsigned int getMaxNumberOfBlocks(){
            return MaxNumberOfBlocks;
        }

    /**
     * @brief Gets the number of used storage blocks.
     * @return The number of used storage blocks.
     */
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