#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "src/Storage.cpp"

void populateData(Storage diskStorage, unsigned int recordLength, string gamesDataFile);
void testInsertDeleteGetMethods (Storage diskStorage, unsigned int recordLength);
void printRecords(Storage diskStorage, unsigned int recordLength);

using namespace std;

int main() {
    unsigned int memorySizeMB = 100;                  // MB
    unsigned int recordLength = sizeof(Record);     // Bytes
    unsigned int blockSize = 60;                    // Bytes
    string gamesDataFile = "games.txt";

    // Allocate Memory
    Storage diskStorage{memorySizeMB*1024*1024, blockSize};
    // Allocate B+ Tree <<< To be coded
    populateData(diskStorage, recordLength,gamesDataFile);
}

void populateData(Storage diskStorage, unsigned int recordLength, string gamesDataFile) {
    Record *dataRecord; 
    ifstream gamesdataFileStream;
    string  GAME_DATE_EST, TEAM_ID_home, PTS_home, FG_PCT_home, FT_PCT_home, FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
    string  gamesDataLine, placeHolderLine;    
    Record record;
    int recCount = 0;
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
                                 
       recordAddress = diskStorage.insertRecord(recordLength,&record);
       //B+ Tree insert with key + record Address  <<< To be coded
       recCount ++;
    //   cout << "Record # " << recCount << "  " << GAME_DATE_EST << " Inserted at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset <<"\n";
    }
    cout << " Number of Records Inserted = " << recCount <<"\n";
    gamesdataFileStream.close();

    testInsertDeleteGetMethods(diskStorage,recordLength);

}

void testInsertDeleteGetMethods (Storage diskStorage, unsigned int recordLength){
    Record record;
    RecordAddress recordAddress;
    Record *dataRecord; 

    // Print all records
    //printRecords(diskStorage,recordLength);
    
    //Delete records
    cout << "Deleting 3rd, 4th, 8th and 10th records \n";
    recordAddress.blockNumber = 2;
    recordAddress.offset= 20;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
    
    recordAddress.blockNumber = 3;
    recordAddress.offset= 0;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
  
    recordAddress.blockNumber = 5;
    recordAddress.offset= 40;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    recordAddress.blockNumber = 7;
    recordAddress.offset= 0;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }
    // Print all records
    //printRecords(diskStorage,recordLength);

    // Try deleting a record that is already deleted

    recordAddress.blockNumber = 5;
    recordAddress.offset= 40;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try deleting a record that is beyond memory boundary

    recordAddress.blockNumber = 99999999;
    recordAddress.offset= 40;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try deleting a record that is beyond current usage of memory (Non-existant record)

    recordAddress.blockNumber = 21234;
    recordAddress.offset= 40;
    if (diskStorage.deleteRecord(recordAddress,recordLength)) {
        cout << " Deleted Record at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset << "\n";
    }

    // Try getting a record that is already delted 
    recordAddress.blockNumber = 7;
    recordAddress.offset= 0;
    dataRecord = diskStorage.getRecord(recordAddress,recordLength);
    if (dataRecord == nullptr) {
        cout << "Success.. Tried to extract 10th record that is already deleted and it returned null ptr\n";
    }
    else {
        cout << "Failure.. Tried to extract 10th record that is already deleted and it returned datarecord\n";
    }

    // Try getting a record that is beyond memory boundary
    recordAddress.blockNumber = 99999999;
    recordAddress.offset= 0;
    dataRecord = diskStorage.getRecord(recordAddress,recordLength);
    if (dataRecord != nullptr) {
        cout << "Failure.. Tried to extract Record beyond memory boundary and got a record\n";
    }

    // Try getting a record that is beyond current usage of memory (Non-existant record)
    recordAddress.blockNumber = 22345;
    recordAddress.offset= 0;
    dataRecord = diskStorage.getRecord(recordAddress,recordLength);
    if (dataRecord != nullptr) {
        cout << "Failure.. Tried to extract Record beyond memory boundary and got a record\n";
    }

}

void printRecords (Storage diskStorage, unsigned int recordLength){
    RecordAddress recordAddress;
    Record *dataRecord; 

    int numRecords = 0;
    recordAddress.blockNumber = 1;
    recordAddress.offset = 0;
    dataRecord = diskStorage.getRecord(recordAddress,recordLength);
    while (dataRecord != nullptr) {
        cout  << ++numRecords << " " << dataRecord->gameDate << " " << 
                 dataRecord->teamID   << " " <<
                 dataRecord->homePts  << " " <<
                 dataRecord->FG_PCT_home << " " <<
                 dataRecord->FT_PCT_home << " " <<
                 dataRecord->FG3_PCT_home << " " <<
                 dataRecord->AST_home << " " <<
                 dataRecord->REB_home << " " <<
                 dataRecord->HOME_TEAM_WINS << " " << "\n";
        recordAddress = diskStorage.getNextRecordAddress(recordAddress, recordLength);
        if (recordAddress.blockNumber != -1) {
            dataRecord = diskStorage.getRecord(recordAddress,recordLength);                 
        }
        else{
            dataRecord = nullptr;
            cout << "Reached End of Records !!\n";
        }

    }

}