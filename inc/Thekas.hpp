#pragma once
#include <string>
#include <map>
#include <vector>

#define NUM_THEKAS 9

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
        
        thekas[0].name = " teental  ";
        thekas[0].numBeats = 16;
        thekas[0].bols = {"Dha","Dhin","Dhin","Dha","Dha","Dhin","Dhin","Dha","Dha","Tin","Tin","Ta","Na","Dhin","Dhin","Dha"};        
        thekas[0].accents = {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0};

        thekas[1].name = " rupaktal ";
        thekas[1].numBeats = 7;
        thekas[1].bols = {"Tin","Tin","Na","Dhin","Na","Dhin","Na"};
        thekas[1].accents = {1,0,0,1,0,1,0,0};

        thekas[2].name = " jhaptal  ";
        thekas[2].numBeats = 10;
        thekas[2].bols = {"Dhin","Na","Dhin","Dhin","Na","Tin","Na","Dhin","Dhin","Na"};
        thekas[2].accents = {1,0,1,0,0,0,0,1,0,0};

        thekas[3].name = "  dadra   ";
        thekas[3].numBeats = 6;
        thekas[3].bols = {"Dha","Dhin","Na","Dha","Tin","Na"};
        thekas[3].accents = {1,0,0,0,0,0};

        thekas[4].name = "  ek tal  ";
        thekas[4].numBeats = 12;
        thekas[4].bols = {"Dhin","Dhin","DhaGe","TiRaKiTa","Tun","Na","Ke","Tin","DhaGe","TiRaKiTa","Dhin","Na"};
        thekas[4].accents = {1,0,0,0,1,0,0,0,1,0,1,0};

        thekas[5].name = " tilwada  ";
        thekas[5].numBeats = 16;
        thekas[5].bols = {"Dha","TiRaKiTa","Dhin","Dhin","Dha","Dha","Tin","Tin","Ta","TiRaKiTa","Dhin","Dhin","Dha","Dha","Dhin","Dhin"};
        thekas[5].accents = {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0};
        
        thekas[6].name = " jhoomra  ";
        thekas[6].numBeats = 14;
        thekas[6].bols = {"Dhin","Dha","TiRaKiTa","Dhin","Dhin","DhaGe","TiRaKiTa","Tin","Ta","TiRaKiTa","Dhin","Dhin","DhaGe","TiRaKiTa"};
        thekas[6].accents = {1,0,0,1,0,0,0,0,0,0,1,0,0,0};

        thekas[7].name = " keherwa  ";
        thekas[7].numBeats = 8;
        thekas[7].bols = {"Dha","Ge","Na","Ti","Na","Ke","Dhin","Na"};
        thekas[7].accents = {1,0,0,0,1,0,0,0};

        thekas[8].name = "adachautal";
        thekas[8].numBeats = 14;
        thekas[8].bols = {"Dhin","Dha","TiRaKiTa","Dhin","Dhin","DhaGe","TiRaKiTa","Tin","Ta","TiRaKiTa","Dhin","Dhin","DhaGe","TiRaKiTa"};
        thekas[8].accents = {1,0,1,0,0,0,1,0,0,0,1,0,0,0};

    }

};