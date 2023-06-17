#include "Glide.hpp"
#include <cmath>

void Glide::Init(float sample_rate, int type) {

    sample_rate_ = sample_rate;
    type_ = type;
    current_sample_ = 0;
    current_state_ = false;
    duration_ = 0.250f;
    elapsed_ = float(current_sample_) * (1/float(sample_rate_));
    total_samples_ = duration_ * sample_rate_;
}

void Glide::SetFreqs(float startingFreq, float endingFreq) {

    startfreq_ = startingFreq;
    endfreq_ = endingFreq;
    freqdiff_ = endfreq_ - startfreq_;

}

float Glide::Process() {

    ++current_sample_;

    elapsed_ = float(current_sample_) * (1/float(sample_rate_));

    if (type_ == 0) {

        float val = (float(current_sample_) + 1)/float(total_samples_);
        currentvalue_ = (val * freqdiff_) + startfreq_;

    } else if (type_== 1) {

        float x = (float(current_sample_)+(total_samples_/10))/(total_samples_/10);
        float val  = std::log10(x);
        if (val > 1) {val = 1;}
        currentvalue_ = (val * freqdiff_) + startfreq_;

    }

    if (current_state_) {
        if (elapsed_ >= duration_) {

            float x = (float(current_sample_)+(total_samples_/10))/(total_samples_/10);
            float val  = std::log10(x);
            if (val > 1) {val = 1;}

            current_state_ = false;
        }
    }

    
    return currentvalue_;

}

int Glide::GetCurrentSample() {

    return current_sample_;

}

void Glide::ReTrigger() {

    current_sample_ = 0;
    current_state_ = true;
    elapsed_ = 0.f;

}
    
void Glide::SetDuration(float dur) {

    duration_ = dur;
    total_samples_ = duration_ * sample_rate_;
}

float Glide::GetDuration() {

    return duration_;

}

float Glide::GetElapsed() {

    return elapsed_;

}

bool Glide::GetCurrentState() {

    return current_state_;

}

void Glide::SetCurrentState(bool state) {
    current_state_ = state;
}

float Glide::GetCurrentFreq() {

    return currentvalue_;

}