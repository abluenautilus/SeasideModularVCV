#pragma once
#include <string>
#include <map>
#include <vector>

//SEMITONE KEY FOR REFERENCE
// 1 C
// 2 C#
// 3 D
// 4 Eb
// 5 E
// 6 F
// 7 F#
// 8 G
// 9 G#
//10 A
//11 Bb
//12 B

const std::map<int,std::string> scaleNames {
    {1,"Major"},
    {2,"Natural Minor"},
    {3,"Harmonic Minor"},
    {4,"Major Pentatonic"},
    {5,"Minor Pentatonic"},
    {6,"Dorian"},
    {7,"Mixolydian"},
    {8,"Chromatic"},
    {9,"Tuning"},
    {10,"Bhairavi"},
    {11,"Ahir Bhairav"}
};

const std::map<int,std::string> saReGaMa {
    {1,"Sa"},
    {2,"Re ♭"},
    {3,"Re"},
    {4,"Ga ♭"},
    {5,"Ga"},
    {6,"Ma"},
    {7,"Ma #"},
    {8,"Pa"},
    {9,"Dha ♭"},
    {10,"Dha"},
    {11,"Ni ♭"},
    {12,"Ni"}
};

const std::map<int,std::string> semitoneNames {
    {1,"C"},
    {2,"C#"},
    {3,"D"},
    {4,"E♭"},
    {5,"E"},
    {6,"F"},
    {7,"F#"},
    {8,"G"},
    {9,"G#"},
    {10,"A"},
    {11,"B♭"},
    {12,"B"}
};

const std::map<std::string, std::vector<int>> scaleTones {   

    {"Major", std::vector<int>{1,3,5,6,8,10,12}},
    {"Natural Minor", std::vector<int>{1,3,4,6,8,9,11}},
    {"Harmonic Minor", std::vector<int>{1,3,4,6,8,9,12}},
    {"Major Pentatonic", std::vector<int>{1,3,5,8,10}},
    {"Minor Pentatonic", std::vector<int>{1,4,6,8,11}},
    {"Dorian", std::vector<int>{1,3,4,6,8,10,11}},
    {"Mixolydian", std::vector<int>{1,3,5,6,8,10,11}},
    {"Chromatic", std::vector<int>{1,2,3,4,5,6,7,8,9,10,11,12}},
    {"Tuning", std::vector<int>{1}},
    {"Bhairavi", std::vector<int>{1,2,4,6,8,9,11}},
    {"Ahir Bhairav", std::vector<int>{1,2,5,6,8,10,11}}
};

const std::map<std::string, std::vector<int>> scaleToneWeights {   

    {"Major", std::vector<int>{3,2,3,3,3,2,1}},
    {"Natural Minor", std::vector<int>{3,2,3,3,3,2,1}},
    {"Harmonic Minor", std::vector<int>{3,2,3,3,3,2,1}},
    {"Major Pentatonic", std::vector<int>{2,1,1,1,1}},
    {"Minor Pentatonic", std::vector<int>{1,1,1,1,1}},
    {"Dorian", std::vector<int>{3,2,3,3,3,2,1}},
    {"Mixolydian", std::vector<int>{3,2,3,3,3,2,1}},
    {"Chromatic", std::vector<int>{1,1,1,1,1,1,1,1,1,1,1,1}},
    {"Tuning", std::vector<int>{1}},
    {"Bhairavi", std::vector<int>{3,1,1,1,2,1,1}},
    {"Ahir Bhairav", std::vector<int>{3,1,1,1,2,1,1}}
};
