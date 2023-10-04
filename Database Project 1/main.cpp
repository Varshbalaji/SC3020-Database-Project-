#include <iostream>
#include <fstream>
#include <string>
#include "src/Storage.cpp"

void populateData(Storage diskStorage, unsigned int recordLength);
void printRecords(Storage diskStorage, unsigned int recordLength);

using namespace std;

int main() {
    unsigned int memorySizeMB = 100;                  // MB
    unsigned int recordLength = sizeof(Record);     // Bytes
    unsigned int blockSize = 60;                    // Bytes

    // Allocate Memory
    Storage diskStorage{memorySizeMB*1024*1024, blockSize};
    // Allocate B+ Tree <<< To be coded
    populateData(diskStorage, recordLength);
}

void populateData(Storage diskStorage, unsigned int recordLength) {
    Record *dataRecord; 
    ifstream dataFile("games_Small.txt");
    string GAME_DATE_EST;
    string TEAM_ID_home;
    string PTS_home;
    string FG_PCT_home;
    string FT_PCT_home;
    string FG3_PCT_home;
    string AST_home;
    string REB_home;
    string HOME_TEAM_WINS;
    Record record;
    int i=1, recCount = 0;
    RecordAddress recordAddress;
    while(dataFile >> GAME_DATE_EST >> TEAM_ID_home >> PTS_home >> FG_PCT_home >> FT_PCT_home >> FG3_PCT_home >> AST_home >> REB_home >> HOME_TEAM_WINS) // for each record
    {
       if(i == 1){
        i=0;
        continue;
       }
        strcpy(record.gameDate,GAME_DATE_EST.c_str());
        record.teamID = stol(TEAM_ID_home);
        record.homePts = stoi(PTS_home);
        record.FG_PCT_home = stof(FG_PCT_home);
        record.FT_PCT_home = stof(FT_PCT_home);
        record.FG3_PCT_home = stof(FG3_PCT_home);
        record.AST_home = stoi(AST_home);
        record.REB_home = stoi(REB_home);
        record.HOME_TEAM_WINS = stoi(HOME_TEAM_WINS);
        recordAddress = diskStorage.insertRecord(recordLength,&record);
        //B+ Tree insert with key + record Address  <<< To be coded
        recCount ++;
        //cout << "Record # " << ++recCount << "  " << GAME_DATE_EST << " Inserted at Block " << recordAddress.blockNumber << " Offset " << recordAddress.offset <<"\n";
    }
    cout << " Number of Records Inserted = " << recCount <<"\n";
    dataFile.close();

    // Print all records
    printRecords(diskStorage,recordLength);
    
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
    printRecords(diskStorage,recordLength);

    recordAddress.blockNumber = 7;
    recordAddress.offset= 0;
    dataRecord = diskStorage.getRecord(recordAddress,recordLength);
    if (dataRecord == nullptr) {
        cout << "Success.. Tried to extract 10th record that is already deleted and it returned null ptr\n";
    }
    else {
        cout << "Failure.. Tried to extract 10th record that is already deleted and it returned datarecord\n";
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