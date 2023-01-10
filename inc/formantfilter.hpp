#pragma once
#include "DaisySP/Source/daisysp.h"

using namespace daisysp;

class FormantFilter {
    public:

        Svf filterbank[5];

        FormantFilter() {};
        ~FormantFilter() {};

        void Init(float samplerate, int start, int middle, int end);

        void SetRes(float res);

        void SetDrive(float drive);

        float Process(float input);

        void SetVowel(int vowel);

        void SetStartEndVowels (int vowelstart, int vowelend);

        void Reset();

        void SetDuration(float dur);
        
    private:

        // bass
        // a, e, i, o, u
        float formant_fc_[5][5] = {
        {600, 400, 250, 400, 350},
        {1040, 1620, 1750, 750, 600},
        {2250, 2400, 2600, 2400, 2400},
        {2450, 2800, 3050, 2600, 2675},
        {2750, 3100, 3340, 2900, 2950}
        };

        float formant_bw_[5][5] = {
        {60, 40, 60, 40, 40},
        {70, 80, 90, 80, 80},
        {110, 100, 100, 100, 100},
        {120, 120, 120, 120, 120},
        {130, 120, 120, 120, 120}
        };

        // float formant_gain_[5][5] = {
        // {0, 0, 0, 0, 0, },
        // {-7, -12, -30, -11, -20},
        // {-9, -9, -16, -21, -32},
        // {-12, -12, -22, -20, -28},
        // {-18, -18, -28, -40, -36}
        // };


        float mix_[5] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};

        float res_, drive_, samplerate_, ttotal_, t_, dur_, tick_, div_factor_;
        int current_vowel_, start_vowel_, end_vowel_, middle_vowel_, current_section_;
};