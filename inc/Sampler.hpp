#pragma once
#include <string>
#include "../inc/AudioFile.h"

class Sampler
{
    public:
        Sampler(void) {}
        ~Sampler(void) {}

        std::string audio_filename;
        float sampleRate;

        void Init(float sampleRate);

        void load(std::string filename);

        void resample(float sampleRate);

        void play();

        void stop();

        float getNext();

        bool isPlaying;

        AudioFile<float> audioFile;

    private:
        float sampleRate_;
        std::string filename_;
        int current_sample_;
        int num_samples_;
        std::vector<float> left_buffer_;
        std::vector<float> right_buffer_;
    
};