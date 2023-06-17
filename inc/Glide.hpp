#pragma once
#include <stdint.h>
#include "plugin.hpp"

class Glide {

//Gate envelope with re-settable duration

 public:
    Glide() {}
    ~Glide() {}

    // type: 0 = linear, 1 = logarithmic
    void Init(float sample_rate, int type);

    void SetFreqs(float startingFreq, float endingFreq);

    void ReTrigger();
    
    void SetDuration(float dur);

    bool GetCurrentState();

    void SetCurrentState(bool state);

    float Process();

    float GetDuration();

    float GetElapsed();
    
    float GetCurrentFreq();

    int GetCurrentSample();

private:

    int         sample_rate_,current_sample_,type_,total_samples_;
    float       duration_,elapsed_,currentvalue_,startfreq_,endfreq_,freqdiff_;
    bool        current_state_;

};