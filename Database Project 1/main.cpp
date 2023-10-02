#include <iostream>
#include <fstream>
#include <string>
#include "include/Storage.h"
// #include "Storage.h"
using namespace std;
int main() {

   ifstream dataFile("games.txt");
    string GAME_DATE_EST;
    string TEAM_ID_home;
    string PTS_home;
    string FG_PCT_home;
    string FT_PCT_home;
    string FG3_PCT_home;
    string AST_home;
    string REB_home;
    string HOME_TEAM_WINS;
    vector <Record> input_buffer;
    int i=1;
    while(dataFile >> GAME_DATE_EST >> TEAM_ID_home >> PTS_home >> FG_PCT_home >> FT_PCT_home >> FG3_PCT_home >> AST_home >> REB_home >> HOME_TEAM_WINS) // for each record
    {
        Record r;
        // r.gameDate = stoi(GAME_DATE_EST.substr(0,2))*1000000 + stoi(GAME_DATE_EST.substr(3,2))*10000+stoi(GAME_DATE_EST.substr(6,4));
       if(i == 1){
        i=0;
        continue;
       }
    //    r.recordAddress = (RecordRow)malloc(sizeof(RecordRow));
       r.teamID = stoi(TEAM_ID_home)%10000;
        r.homePts = stoi(PTS_home);
        r.FG_PCT_home = stof(FG_PCT_home)*1000;
        r.FT_PCT_home = stof(FT_PCT_home)*1000;
        r.FG3_PCT_home = stof(FG3_PCT_home)*1000;
        r.AST_home = stoi(AST_home);
        r.REB_home = stoi(REB_home);
        r.HOME_TEAM_WINS = stoi(HOME_TEAM_WINS);
        input_buffer.push_back(r);
    }
    cout<< "Reading data is done"<<endl;
    dataFile.close();
    


}
