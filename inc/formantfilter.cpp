#include "plugin.hpp"
#include "formantfilter.hpp"

#define NUM_FILTERS 5


float lerp(float a, float b, float f)
{
    // simple linear interpolation
    return a * (1.0 - f) + (b * f);
}

void FormantFilter::Init(float samplerate, int start, int middle, int end) {

    res_ = 0.98;
    drive_ = 0.02;
    samplerate_ = samplerate;
    dur_ = 2.5;
    ttotal_ = samplerate * dur_;
    t_ = 0;
    start_vowel_ = start;
    end_vowel_ = end;
    current_section_ = 0;
    tick_ = 0;
    div_factor_ = 5;

    for (int i = 0; i < NUM_FILTERS; ++i) {

        filterbank[i].Init(samplerate);
        filterbank[i].SetFreq(formant_fc_[start_vowel_][i]);
        filterbank[i].SetRes(formant_fc_[start_vowel_][i]/formant_bw_[start_vowel_][i]);  // res = fc/bw
        filterbank[i].SetDrive(drive_);

    }
    current_vowel_ = start_vowel_;
    
};

void FormantFilter::SetRes(float res) {

    for (int i = 0; i < NUM_FILTERS; ++i) {
        filterbank[i].SetRes(res);
    }

};

void FormantFilter::SetDrive(float drive) {

    for (int i = 0; i < NUM_FILTERS; ++i) {
        filterbank[i].SetDrive(drive);
    }

};

void FormantFilter::SetVowel(int vowel) {
    current_vowel_ = vowel;

        for (int i = 0; i < NUM_FILTERS; ++i) {

            filterbank[i].SetFreq(formant_fc_[vowel][i]);
            filterbank[i].SetRes(formant_fc_[vowel][i]/formant_bw_[start_vowel_][i]);  // res = fc/bw
        }
};

float FormantFilter::Process(float input) {

    float output = 0;
    int vowel_from, vowel_to;

    if (t_ > (dur_/2)) {
        current_section_ = 1;
    }

    if (current_section_ == 0) {
        vowel_from = start_vowel_;
        vowel_to = middle_vowel_;
    } else {
        vowel_from = middle_vowel_;
        vowel_to = end_vowel_;
    }

    for (int i = 0; i < NUM_FILTERS; ++i) {

        
        float currentpoint = t_/ttotal_;
        // interpolate between start and end

        //reduce processing load by interpolating less frequently
        if (tick_== div_factor_) {
            float freq = lerp(formant_fc_[vowel_from][i], formant_fc_[vowel_to][i], currentpoint);
            float bw = lerp(formant_bw_[vowel_from][i], formant_bw_[vowel_to][i], currentpoint);
            filterbank[i].SetFreq(freq);
            filterbank[i].SetRes(freq/bw);
        } 

        ++tick_;
        if (tick_ > div_factor_) {tick_ = 0;}

        // process filter
        filterbank[i].Process(input);
        output += filterbank[i].Band() * mix_[i];

    };


    ++t_; 

    return output;

};

void FormantFilter::SetStartEndVowels(int vowela, int vowelb) {

    start_vowel_ = vowela;
    end_vowel_ = vowelb;

}

void FormantFilter::SetDuration(float dur) {
    dur_ = dur;
    ttotal_ = samplerate_ * dur_;
}

void FormantFilter::Reset() {

    t_ = 0;
    current_section_ = 0;

}