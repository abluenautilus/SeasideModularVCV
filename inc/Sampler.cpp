#include <string>
#include "Sampler.hpp"
#include "plugin.hpp"

void Sampler::Init(float sampleRate) {

    sampleRate_ = sampleRate;

}

void Sampler::load(std::string filename) {

    filename_ = filename;

    if (audioFile.load(filename_)) {
        INFO("Loaded file %s",filename_.c_str());
    } else {
        INFO("Error loading file %s",filename_.c_str());
    }

    uint32_t sampleRate = audioFile.getSampleRate();
    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();

    num_samples_ = numSamples;

    INFO("SampleRate %d numSamples %d numchannels %d",sampleRate,numSamples,numChannels);

    for (int i =0; i < numSamples; ++i) {
        left_buffer_.push_back(audioFile.samples[0][i]);
        right_buffer_.push_back(audioFile.samples[0][i]);
    }

}

void Sampler::play() {
    isPlaying = true;
    current_sample_ = 0;
}

float Sampler::getNext() {

    float out = 0.0;
    if (current_sample_ < num_samples_ && isPlaying) {

        out = left_buffer_[current_sample_];
        ++current_sample_;
        
    } else {
        isPlaying = false;
    }

    return out;    
}
