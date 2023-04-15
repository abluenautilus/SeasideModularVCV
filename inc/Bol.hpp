#pragma once
#include <string>
#include <vector>
#include <map>
#include "AudioFile.h"
#include "plugin.hpp"
#include "Sample.h"

std::string BOLS[14] = {"Dha","Dhin","Dhit","Dhun","Ga","Ge","Ke","Na","Ta","Ti","Tin","Tun","TiRaKiTa","DhaGe"};
const std::map<std::string, std::vector<std::string>> fileNames {   
   
    {"Dha", std::vector<std::string>{"Dha01.wav","Dha02.wav"}},
    {"Dhin", std::vector<std::string>{"Dhin01.wav","Dhin02.wav","Dhin03.wav","Dhin04.wav"}},
    {"Dhit", std::vector<std::string>{"Dhit01.wav","Dhit02.wav","Dhit03.wav","Dhit04.wav"}},
    {"Dhun", std::vector<std::string>{"Dhun01.wav","Dhun02.wav","Dhun05.wav","Dhun06.wav"}},
    {"Ga", std::vector<std::string>{"Ga01.wav","Ga02.wav","Ga03.wav","Ga04.wav"}},
    {"Ge", std::vector<std::string>{"Ge01.wav","Ge02.wav","Ge03.wav","Ge04.wav"}},
    {"Ke", std::vector<std::string>{"Ke01.wav","Ke04.wav","Ke05.wav","Ke06.wav"}},
    {"Na", std::vector<std::string>{"Na01.wav","Na02.wav","Na04.wav","Na05.wav"}},
    {"Ta", std::vector<std::string>{"Ta01.wav","Ta10.wav"}},
    {"Ti", std::vector<std::string>{"Ti01.wav","Ti06.wav","Ti07.wav","Ti09.wav"}},
    {"Tin", std::vector<std::string>{"Tin01.wav","Tin01a.wav"}},
    {"Tun", std::vector<std::string>{"Tun01.wav","Tun02.wav","Tun03.wav","Tun07.wav"}},
    {"TiRaKiTa", std::vector<std::string>{"TiRaKiTa01.wav"}},
    {"DhaGe",std::vector<std::string>{"DhaGe01.wav"}}
};

const std::map<std::string,int> bolNums {
    {"Dha",0},
    {"Dhin",1},
    {"Dhit",2},
    {"Dhun",3},
    {"Ga",4 },
    {"Ge",5 },
    {"Ke",6 },
    {"Na",7},
    {"Ta",8 },
    {"Ti",9 },
    {"Tin",10},
    {"Tun",11},
    {"TiRaKiTa",12},
    {"DhaGe",13}
};


class Bol
{
public:

    ~Bol(void) {}

    std::string bolName;
    int id;
    int num_sounds;
    int last_played;
    int current_sample_num = 0;
    int mode; // 0 = first sample only, 1 = round robin, 2 = random
    bool isPlaying = false;
    bool isReadyToPlay = false;
    Sample current_sample;

    std::vector<Sample> samples;

    Bol() {}

    Bol(std::string initname, float *buff) {

        bolName = initname;

        //clear vectors
        std::vector<Sample>().swap(samples);
    
        //Load in samples
        loadSamples(buff);
    }

    int loadSamples(float *buff) {
        
        isReadyToPlay = false;
        int success = 1;

        std::vector<std::string> filesToLoad = fileNames.at(bolName);
        num_sounds = static_cast<int>(filesToLoad.size());

        for (int i = 0; i < num_sounds; i++) {

            std::string filePath = rack::asset::plugin(pluginInstance,"res/bols/" + bolName + "/"+ filesToLoad.at(i));
            samples.push_back(Sample(filePath, buff));

        }
        isReadyToPlay = true;
        return success;

    }

    void reLoad() {
        
        isReadyToPlay = false;
        for (int i = 0; i < num_sounds; i++) {
            samples.at(i).load();
        }
        isReadyToPlay = true;
    }

    void Play() {

        isPlaying = true;
        switch(mode) {
         case(0):
            current_sample_num = 0;
            break;
         case(1):
            ++current_sample_num;
            break;
         case(2):
            current_sample_num = rand() % (num_sounds-1);
            break;
        }
    
        if (current_sample_num >= num_sounds) {current_sample_num = 0;}
        if (current_sample_num < 0 ) {current_sample_num = num_sounds-1;}

        current_sample = samples[current_sample_num];
        current_sample.currentSample = 0;
        
    }

    void Stop() {
        isPlaying = false;
    }


};


