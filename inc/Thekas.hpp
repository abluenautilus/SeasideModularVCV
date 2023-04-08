#pragma once
#include <string>
#include <map>
#include <vector>

#define NUM_THEKAS 4

struct Theka {

    int numBeats;
    std::string name;
    std::vector<std::string> bols;
    std::vector<int> accents;

};

class ThekaLibrary
{
public:
    Theka thekas[NUM_THEKAS];

    ThekaLibrary() {    
        
        thekas[0].name = "teental ";
        thekas[0].numBeats = 16;
        thekas[0].bols = {"Dha","Dhin","Dhin","Dha","Dha","Dhin","Dhin","Dha","Dha","Tin","Tin","Ta","Na","Dhin","Dhin","Dha"};        
        thekas[0].accents = {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0};

        thekas[1].name = "rupaktal";
        thekas[1].numBeats = 7;
        thekas[1].bols = {"Tin","Tin","Na","Dhin","Na","Dhin","Na"};
        thekas[1].accents = {1,0,0,1,0,1,0,0};

        thekas[2].name = "jhaptal ";
        thekas[2].numBeats = 10;
        thekas[2].bols = {"Dhin","Na","Dhin","Dhin","Na","Tin","Na","Dhin","Dhin","Na"};
        thekas[2].accents = {1,0,1,0,0,0,0,1,0,0};

        thekas[3].name = " dadra  ";
        thekas[3].numBeats = 6;
        thekas[3].bols = {"Dha","Dhin","Na","Dha","Tin","Na"};
        thekas[3].accents = {1,0,0,0,0,0};

    }

};