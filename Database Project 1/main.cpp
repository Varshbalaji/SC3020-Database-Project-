#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>

#include "src/Storage.cpp"
#include "src/BTree_insert.cpp"

void experiment_1(Storage *diskStorage,  unsigned int recordLength, string gamesDataFile, unsigned int blockSize);
//void experiment_2(Storage *diskStorage, Btree *tree, unsigned int recordLength);
void experiment_3(Storage *diskStorage, unsigned int recordLength);
void experiment_4(Storage *diskStorage, unsigned int recordLength);
void experiment_5(Storage *diskStorage, unsigned int recordLength);

void populateData(Storage *diskStorage, unsigned int recordLength, string gamesDataFile);
void testInsertDeleteGetMethods (Storage *diskStorage, unsigned int recordLength);
void printRecords(Storage *diskStorage, unsigned int recordLength);

//For Debugging purposes
void printKeys(Storage *diskStorage, unsigned int recordLength);
void printKeysforBtreeValidation();

using namespace std;

int main() {
    unsigned int memorySizeMB = 100;                  // MB
    unsigned int recordLength = sizeof(Record);     // Bytes
    unsigned int blockSize = 400;                    // Bytes
    string gamesDataFile = "games.txt";

    // Debug purposes
    // printKeys(&diskStorage,recordLength);
    // printKeysforBtreeValidation();
    // Allocate Memory
    
    Storage diskStorage{memorySizeMB*1024*1024, blockSize};
    Btree tree(&diskStorage);

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "*                                        Storage Stats                                               *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";
    cout << " Memory Allocated                                 = " << memorySizeMB << " MB\n";
    cout << " Block Size                                       = " << blockSize << " B\n";
    cout << " Number of Blocks Allocated                       = " << diskStorage.getMaxNumberOfBlocks() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";

    // Experiment 1: Store the NBA games data on the disk
    experiment_1(&diskStorage, recordLength, gamesDataFile, blockSize);
    
    // Experiment 2: Build a B+ tree on the attribute "FG_PCT_home"
    //experiment_2(&diskStorage, &tree, recordLength);
    
    //Sequential scan Storage to get records and build B+ Tree

    RecordAddress recordAddress;
    Record *dataRecord; 

    int numberOfRecords = 0, numberOfDataBlocksAccessed = 0, totalNumberOfDataBlocksAccessed = 0 ;

    recordAddress.blockNumber = 1; // Start with first record at Block 1 : Offset 0
    recordAddress.offset = 0;

    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage.getRecord(recordAddress,recordLength);
    tree.insert(dataRecord->FG_PCT_home, recordAddress);

    while (dataRecord != nullptr) {

        tree.insert(dataRecord->FG_PCT_home, recordAddress);

        numberOfRecords++;
        totalNumberOfDataBlocksAccessed +=  numberOfDataBlocksAccessed;
        
        recordAddress = diskStorage.getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {    // Not End of File
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage.getRecord(recordAddress,recordLength);        
        }
        else{
            dataRecord = nullptr;
        }
    }
    cout << "number of records"<< numberOfRecords << "\n\n";
    cout << "TREE START..."<<endl;

   
    tree.printTree(tree.fetchRoot());
    cout << "RANGE" << "\n";
    vector<Key_Records> searchResult = tree.search(0.6,true,1);
    cout << "lalala"<<searchResult.size()<<"\n";
    for(int i=0; i<searchResult.size(); i++){
        cout<<searchResult[i].key_value<<"\n";
    }
    float average = tree.avgFG3_PCT_home(&diskStorage,searchResult);
    cout<<"AVERAGE: "<<average<<"\n";

    cout << "VERFYYYY"<<"\n";
    BTreeNode* result = tree.fetchSmallestLeaf(tree.root);
    for(int i=0;i<tree.deg;i++){
            Key_Records value = result->keys[i];
    }





    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "*                    Experiment 2: Build a B+ tree on the attribute FG_PCT_home                      *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";
    cout << " Parameter n of the B+ Tree                       = " << tree.deg << "\n";
    cout << " Number of node of the B+ Tree                    = " << tree.nodeCount << "\n";
    cout << " Number of levels of the B+ Tree                  = " << tree.getLevels(tree.root) << "\n";
    //might change if root node contains more than one key, will have to run a for loop 
    cout << " Content of root node                             = " << tree.root->keys[0].key_value << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";
    
    // Experiment 3: Retrieve the games with "FG_PCT_home" = 0.5 (B+ tree) and calculate average of "FG3_PCT_Home"
    experiment_3(&diskStorage, recordLength);

    // Experiment 4: Retrieve the games with "FG_PCT_home" between 0.6 and 1 (B+ tree) and calculate average of "FG3_PCT_Home"
    experiment_4(&diskStorage, recordLength);

    // Experiment 5: Retrieve the games with "FG_PCT_home" <= 0.35 (B+ tree) and delete qualifying records"
    experiment_5(&diskStorage, recordLength);

    //testInsertDeleteGetMethods(&diskStorage,recordLength);
}

void experiment_1(Storage *diskStorage,  unsigned int recordLength, string gamesDataFile, unsigned int blockSize){

    // Populate NBA games Data from file
    populateData(diskStorage, recordLength,gamesDataFile);

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "*                          Experiment 1: Store the NBA games data on the disk                        *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";
    cout << " Number of Game Records Inserted                  = " << diskStorage->getTotalNumberOfRecords() << "\n";
    cout << " Size of Record                                   = " << recordLength << "\n";
    int numOfRecordsPerBlock = (blockSize / recordLength);
    if ((blockSize % recordLength) > 0){
        numOfRecordsPerBlock++ ; // Accounting for Spanned Records (Partial records)
    }
    cout << " Number of Record Stored in a Block               = " << numOfRecordsPerBlock << " (Spanned Records)\n";
    cout << " Number of Blocks Storing Data                    = " << diskStorage->getUsedBlocks() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";
}

// void experiment_2(Storage *diskStorage, Btree *tree, unsigned int recordLength){

//     //Sequential scan Storage to get records and build B+ Tree



//     RecordAddress recordAddress;
//     Record *dataRecord; 

//     int numberOfRecords = 0, numberOfDataBlocksAccessed = 0, totalNumberOfDataBlocksAccessed = 0 ;

//     recordAddress.blockNumber = 1; // Start with first record at Block 1 : Offset 0
//     recordAddress.offset = 0;

//     tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
//     while (dataRecord != nullptr) {

//         tree->insert(dataRecord->FG_PCT_home, recordAddress);

//         numberOfRecords++;
//         totalNumberOfDataBlocksAccessed +=  numberOfDataBlocksAccessed;
        
//         recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength);
//         if (recordAddress.blockNumber != -1) {    // Not End of File
//             tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);        
//         }
//         else{
//             dataRecord = nullptr;
//         }
//     }
//     cout << "number of records"<< numberOfRecords;


//     cout << "*----------------------------------------------------------------------------------------------------*\n";
//     cout << "*                    Experiment 2: Build a B+ tree on the attribute FG_PCT_home                      *\n";
//     cout << "*----------------------------------------------------------------------------------------------------*\n";
//     cout << "\n";
//     cout << " Parameter n of the B+ Tree                       = " << tree->deg << "\n";
//     cout << " Number of node of the B+ Tree                    = " << tree->nodeCount << "\n";
//     cout << " Number of levels of the B+ Tree                  = " << tree->getLevels(tree->root) << "\n";
//     //might change if root node contains more than one key, will have to run a for loop 
//     cout << " Content of root node                             = " << tree->root->keys[0].key_value << "\n\n";
//     cout << "*----------------------------------------------------------------------------------------------------*\n\n";
// }

void experiment_3(Storage *diskStorage, unsigned int recordLength){

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "   *Experiment 3: Retrieve the games with FG_PCT_home = 0.5  and calculate average of FG3_PCT_home   *\n";
    cout << "                                        B+ Tree Access Method                                        *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";

    float FG_PCT_HOME_Predicate = 0.5;  // Key Predicate value set to 0.5

    //Capture Start Time for Elapsed time calcuation
    auto startClock = chrono::high_resolution_clock::now();

    // B+ Tree based Search to rereive games with FG_PCT_home = 0.5  and calculate average of FG3_PCT_Hhome <<<<< To be coded

    //Capture Start Time for Elapsed time calcuation
    auto endClock = chrono::high_resolution_clock::now();
    chrono::duration< double > elapsed = endClock - startClock;
    chrono::nanoseconds elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );
    
    cout << " Number of index nodes accessed                   = " << "???" << "\n";
    cout << " Number of data blocks accessed                   = " << "???" << "\n";
    cout << " Number of records qualified                      = " << "???" << "\n";
    cout << " Average of FG3_PCT_home of qualified records     = " << "???" << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";    
    
    //Now do the experiment by  bruteforce linear scan method

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "   *Experiment 3: Retrieve the games with FG_PCT_home = 0.5  and calculate average of FG3_PCT_home   *\n";
    cout << "                                     Brute Force Linear Scan Method                                  *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";

    //Capture Start Time for Elapsed time calcuation
    startClock = chrono::high_resolution_clock::now();

    RecordAddress recordAddress;
    Record *dataRecord; 

    int numberOfRecords = 0, numberOfDataBlocksAccessed = 0, totalNumberOfDataBlocksAccessed = 0 ;
    float cumulative_FG3_PCT_home=0, average_FG3_PCT_home;

    recordAddress.blockNumber = 1; // Start with first record at Block 1 : Offset 0
    recordAddress.offset = 0;

    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        totalNumberOfDataBlocksAccessed +=  numberOfDataBlocksAccessed;

        if ( dataRecord->FG_PCT_home == FG_PCT_HOME_Predicate) {
            cumulative_FG3_PCT_home += dataRecord->FG3_PCT_home;
            numberOfRecords++;
        }        
        recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {    // Not End of File
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);        
        }
        else{
            dataRecord = nullptr;
        }
    }
    if (numberOfRecords > 0) {  // Calculate average
        average_FG3_PCT_home = cumulative_FG3_PCT_home / numberOfRecords;
    }
    //Capture Start Time for Elapsed time calcuation
    endClock = chrono::high_resolution_clock::now();
    elapsed = endClock - startClock;
    elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );

    cout << " Number of data blocks accessed                   = " << totalNumberOfDataBlocksAccessed << "\n";
    cout << " Number of records qualified                      = " << numberOfRecords << "\n";
    cout << " Average of FG3_PCT_home of qualified records     = " << average_FG3_PCT_home << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";
}

void experiment_4(Storage *diskStorage, unsigned int recordLength){

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "* Experiment 4: Retrieve the games with FG_PCT_home between 0.6 and 1 (B+ tree) and calculate average*\n";
    cout << "                               of FG3_PCT_Home using B+ Tree Access Method                           *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";

    float FG_PCT_HOME_Predicate_From = 0.6, FG_PCT_HOME_Predicate_To = 1;   // Predicate Range Set between 0.6 and 1 (inclusive )

    //Capture Start Time for Elapsed time calcuation
    auto startClock = chrono::high_resolution_clock::now();

    // B+ Tree based Search to rereive games with FG_PCT_home = 0.5  and calculate average of FG3_PCT_Hhome <<<<< To be coded

    //Capture Start Time for Elapsed time calcuation
    auto endClock = chrono::high_resolution_clock::now();
    chrono::duration< double > elapsed = endClock - startClock;
    chrono::nanoseconds elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );
    
    cout << " Number of index nodes accessed                   = " << "???" << "\n";
    cout << " Number of data blocks accessed                   = " << "???" << "\n";
    cout << " Number of records qualified                      = " << "???" << "\n";
    cout << " Average of FG3_PCT_home of qualified records     = " << "???" << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";    
    
    //Now do the experiment by  bruteforce linear scan method

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "* Experiment 4: Retrieve the games with FG_PCT_home between 0.6 and 1 (B+ tree) and calculate average*\n";
    cout << "                          of FG3_PCT_Home using Brute Force Linear Scan Method                       *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";

    //Capture Start Time for Elapsed time calcuation
    startClock = chrono::high_resolution_clock::now();

    RecordAddress recordAddress;
    Record *dataRecord; 

    int numberOfRecords = 0, numberOfDataBlocksAccessed = 0, totalNumberOfDataBlocksAccessed = 0 ;
    float cumulative_FG3_PCT_home=0, average_FG3_PCT_home;

    recordAddress.blockNumber = 1; // Start with first record at Block 1 : Offset 0
    recordAddress.offset = 0;

    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        totalNumberOfDataBlocksAccessed +=  numberOfDataBlocksAccessed;

        if (dataRecord->FG_PCT_home >= FG_PCT_HOME_Predicate_From && dataRecord->FG_PCT_home <= FG_PCT_HOME_Predicate_To) {
            cumulative_FG3_PCT_home += dataRecord->FG3_PCT_home;
            numberOfRecords++;
        }        

        recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength); 
        if (recordAddress.blockNumber != -1) {    // Not End of File
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);        
        }
        else{
            dataRecord = nullptr;
        }
    }
    if (numberOfRecords > 0) {  // Calculate average
        average_FG3_PCT_home = cumulative_FG3_PCT_home / numberOfRecords;
    }
    //Capture Start Time for Elapsed time calcuation
    endClock = chrono::high_resolution_clock::now();
    elapsed = endClock - startClock;
    elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );

    cout << " Number of data blocks accessed                   = " << totalNumberOfDataBlocksAccessed << "\n";
    cout << " Number of records qualified                      = " << numberOfRecords << "\n";
    cout << " Average of FG3_PCT_home of qualified records     = " << average_FG3_PCT_home << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";
}

void experiment_5(Storage *diskStorage, unsigned int recordLength){

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "*  Experiment 5: Retrieve the games with FG_PCT_home <= 0.35 (B+ tree) and delete qualifying records *\n";
    cout << "                                       using B+ Tree Access Method                                   *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "\n";

    float FG_PCT_HOME_Predicate = 0.35;  // Key Predicate value set to 0.35

    //Capture Start Time for Elapsed time calcuation
    auto startClock = chrono::high_resolution_clock::now();

    // B+ Tree based Search to rereive games with FG_PCT_home = 0.5  and calculate average of FG3_PCT_Hhome <<<<< To be coded

    //Capture Start Time for Elapsed time calcuation
    auto endClock = chrono::high_resolution_clock::now();
    chrono::duration< double > elapsed = endClock - startClock;
    chrono::nanoseconds elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );
    
    cout << " Number of nodes updated                          = " << "???" << "\n";
    cout << " Number of levels updated                         = " << "???" << "\n";
    cout << " Number of records qualified for deletion         = " << "???" << "\n";    
    cout << " Content of root node                             = " << "???" << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";    
    
    //Now do the experiment by  bruteforce linear scan method

    cout << "*----------------------------------------------------------------------------------------------------*\n";
    cout << "*  Experiment 5: Retrieve the games with FG_PCT_home <= 0.35 (B+ tree) and delete qualifying records *\n";
    cout << "                                  using Brute Force Linear Scan Method                               *\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n";

    //Capture Start Time for Elapsed time calcuation
    startClock = chrono::high_resolution_clock::now();

    RecordAddress recordAddress;
    Record *dataRecord; 

    int numberOfRecords = 0, numberOfDataBlocksAccessed = 0, totalNumberOfDataBlocksAccessed = 0 ;
    float cumulative_FG3_PCT_home=0, average_FG3_PCT_home;

    recordAddress.blockNumber = 1; // Start with first record at Block 1 : Offset 0
    recordAddress.offset = 0;

    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        totalNumberOfDataBlocksAccessed +=  numberOfDataBlocksAccessed;            

        if (dataRecord->FG_PCT_home <= FG_PCT_HOME_Predicate ) {
            diskStorage->deleteRecord(recordAddress,recordLength);
            numberOfRecords++;
        }

        recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {    // Not End of File
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);       
        }
        else{
            dataRecord = nullptr;
        }
    }
    if (numberOfRecords > 0) {  // Calculate average
        average_FG3_PCT_home = cumulative_FG3_PCT_home / numberOfRecords;
    }
    //Capture Start Time for Elapsed time calcuation
    endClock = chrono::high_resolution_clock::now();
    elapsed = endClock - startClock;
    elapsedNs = chrono::duration_cast< chrono::nanoseconds >( elapsed );

    cout << " Number of data blocks accessed                   = " << totalNumberOfDataBlocksAccessed << "\n";
    cout << " Number of records qualified for deletion         = " << numberOfRecords << "\n";
    cout << " Running Time (Elapsed in nanosecs)               = " << elapsedNs.count() << "\n\n";
    cout << "*----------------------------------------------------------------------------------------------------*\n\n";
}

void populateData(Storage *diskStorage, unsigned int recordLength, string gamesDataFile) {
    Record *dataRecord; 
    ifstream gamesdataFileStream;
    string  GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home, FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
    string  gamesDataLine, placeHolderLine;    
    Record record;
    //int recCount = 0;
    RecordAddress recordAddress;

    gamesdataFileStream.open(gamesDataFile);
//    while(dataFile >> GAME_DATE_EST >> TEAM_ID_home >> PTS_home >> FG_PCT_home >> FT_PCT_home >> FG3_PCT_home >> AST_home >> REB_home >> HOME_TEAM_WINS) // for each record
    while(getline(gamesdataFileStream,gamesDataLine)) // for each record
    {
       // Initialize String variables
       GAME_DATE_EST = "";
       TEAM_ID_home = "";
       PTS_home = "";
       FG_PCT_home = "";
       FT_PCT_home = "";
       FG3_PCT_home = "";
       AST_home = "";
       REB_home = "";
       HOME_TEAM_WINS= "";

       // Break gamesDataLine into words using stringStream
       stringstream gamesDataLineStream (gamesDataLine);

       getline(gamesDataLineStream,GAME_DATE_EST,'\t'); // First Word is captured in GAME_DATE_EST as there is no tab before first word
       
       gamesDataLineStream >> TEAM_ID_home >> PTS_home >> FG_PCT_home >> FT_PCT_home >> FG3_PCT_home >> AST_home >> REB_home >> HOME_TEAM_WINS;

       if (GAME_DATE_EST == "GAME_DATE_EST") {
           // It is a header line.. Skip processing
           continue;
       }
       strcpy(record.gameDate,GAME_DATE_EST.c_str());

       if (TEAM_ID_home != ""){
        record.teamID = stol(TEAM_ID_home);
       }
       else {
        record.teamID = 0;
       }

       if (PTS_home != "") {
        record.homePts = stoi(PTS_home);
       }
       else {
        record.homePts = 0;
       }       
    
       if (FG_PCT_home != "") {
        record.FG_PCT_home = stof(FG_PCT_home);
       }
       else {
        record.FG_PCT_home = 0;
       }

       if (FT_PCT_home != "") {
        record.FT_PCT_home = stof(FT_PCT_home);
       }
       else {
        record.FT_PCT_home = 0;
       }
       
       if (FG3_PCT_home != "") {
        record.FG3_PCT_home = stof(FG3_PCT_home);
       }
       else {
        record.FG3_PCT_home = 0;
       }
       
       if (AST_home != "") {
        record.AST_home = stoi(AST_home);
       }
       else {
        record.AST_home = 0;
       }
       
       if (REB_home != "") {
        record.REB_home = stoi(REB_home);
       }
       else {
        record.REB_home = 0;
       }
       
       if (HOME_TEAM_WINS != "") {
        record.HOME_TEAM_WINS = stoi(HOME_TEAM_WINS);
        
       }
       else {
        record.HOME_TEAM_WINS = 0;
       }
                                 
       recordAddress = diskStorage->insertRecord(recordLength,&record);
       //B+ Tree insert with key + record Address  <<< To be coded
       //recCount ++;
       //cout << "Record # " << recCount << "  " << GAME_DATE_EST << " Inserted at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset <<"\n";
    }
    //cout << " Number of Records Inserted = " << recCount <<"\n";
    gamesdataFileStream.close();

}

void testInsertDeleteGetMethods (Storage *diskStorage, unsigned int recordLength){
    Record record;
    RecordAddress recordAddress;
    Record *dataRecord; 
    int numberOfDataBlocksAccessed;

    // Print all records
    printRecords(diskStorage,recordLength);
    
    //Delete records
    cout << "Deleting 3rd, 4th, 8th and 10th records \n";
    recordAddress.blockNumber = 1;
    recordAddress.offset= 88;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
    
    recordAddress.blockNumber = 1;
    recordAddress.offset= 132;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
  
    recordAddress.blockNumber = 1;
    recordAddress.offset= 308;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    recordAddress.blockNumber = 1;
    recordAddress.offset= 396;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
    // Print all records
    //printRecords(diskStorage,recordLength);

    // Try deleting a record that is already deleted

    recordAddress.blockNumber = 1;
    recordAddress.offset= 396;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try deleting a record that is beyond memory boundary

    recordAddress.blockNumber = 99999999;
    recordAddress.offset= 40;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try deleting a record that is beyond current usage of memory (Non-existant record)

    recordAddress.blockNumber = 21234;
    recordAddress.offset= 40;
    if (diskStorage->deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try getting a record that is already delted 
    recordAddress.blockNumber = 1;
    recordAddress.offset= 308;
    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    if (dataRecord == nullptr) {
        cout << "Success.. Tried to extract 10th record that is already deleted and it returned null ptr\n";
    }
    else {
        cout << "Failure.. Tried to extract 10th record that is already deleted and it returned datarecord\n";
    }

    // Try getting a record that is beyond memory boundary
    recordAddress.blockNumber = 99999999;
    recordAddress.offset= 0;
    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    if (dataRecord != nullptr) {
        cout << "Failure.. Tried to extract Record beyond memory boundary and got a record\n";
    }

    // Try getting a record that is beyond current usage of memory (Non-existant record)
    recordAddress.blockNumber = 22345;
    recordAddress.offset= 0;
    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    if (dataRecord != nullptr) {
        cout << "Failure.. Tried to extract Record beyond memory boundary and got a record\n";
    }

}

void printRecords (Storage *diskStorage, unsigned int recordLength){
    RecordAddress recordAddress;
    Record *dataRecord; 

    int numRecords = 0, numberOfDataBlocksAccessed;
    recordAddress.blockNumber = 1;
    recordAddress.offset = 0;
    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        cout  << ++numRecords << " " << dataRecord->gameDate << " " << 
                 dataRecord->teamID   << " " <<
                 dataRecord->homePts  << " " <<
                 dataRecord->FG_PCT_home << " " <<
                 dataRecord->FT_PCT_home << " " <<
                 dataRecord->FG3_PCT_home << " " <<
                 dataRecord->AST_home << " " <<
                 dataRecord->REB_home << " " <<
                 dataRecord->HOME_TEAM_WINS << " " <<
                 "Number of data blocks Accessed = " << numberOfDataBlocksAccessed << "\n";
        recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);             
        }
        else{
            dataRecord = nullptr;
            cout << "Reached End of Records !!\n";
        }

    }
}
void printKeys (Storage *diskStorage, unsigned int recordLength){
    RecordAddress recordAddress;
    Record *dataRecord; 

    ofstream KeyFile("printKeys.txt");

    int numRecords = 0, numberOfDataBlocksAccessed;
    recordAddress.blockNumber = 1;
    recordAddress.offset = 0;
    tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        KeyFile  <<  setprecision (3) << fixed  << dataRecord->FG_PCT_home  << "  " << dataRecord->gameDate << " " << dataRecord->teamID   << "\n";
        recordAddress = diskStorage->getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress,recordLength);             
        }
        else{
            dataRecord = nullptr;
            cout << "Reached End of Records !!\n";
        }

    }
    KeyFile.close();

}

void printKeysforBtreeValidation(){
    Record *dataRecord; 
    ifstream printkeysFileStream("SortedprintKeys.txt");
    ofstream KeyFile("printKeysforBtreeValidation.txt");

    string  GAME_DATE_EST, TEAM_ID_home, FG_PCT_home;
    string prevkey = " ";
    while(printkeysFileStream >> FG_PCT_home >> GAME_DATE_EST >> TEAM_ID_home ) {
        if (prevkey != FG_PCT_home){
            if (prevkey != " " ){
                KeyFile <<  " ]\n";
            }
            KeyFile << FG_PCT_home << " [ ";
            prevkey = FG_PCT_home;
        }
        
        KeyFile <<  "("  << GAME_DATE_EST <<',' << TEAM_ID_home << ") ; ";

    }
    printkeysFileStream.close();
    KeyFile.close();
}