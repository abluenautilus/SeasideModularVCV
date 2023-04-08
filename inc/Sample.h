#include "AudioFile.h"
#include "plugin.hpp"
#include <string>
#include <vector>

struct audioSample {
    float left;
    float right;
};

class Sample
{
public:

    std::string filePath;
    bool isLoaded = false;
    std::vector<float> left_audio_buffer;
    std::vector<float> right_audio_buffer;
    int numChannels;
    float sampleRate;
    int numSamples;
    AudioFile<float> audioFile;
    int currentSample = 0;

    Sample(){}

    Sample(std::string filePathInit) {
        filePath = filePathInit;
        clearBuffers();
        load();
    }

    int load() {

        int success = 1;

        if (audioFile.load(filePath)) {
            INFO("Loaded file %s",filePath.c_str());
        } else {
            INFO("Error loading file %s",filePath.c_str());
            success = 0;
        }

        sampleRate = audioFile.getSampleRate();
        numSamples = audioFile.getNumSamplesPerChannel();
        numChannels = audioFile.getNumChannels();

        INFO("SampleRate %.2f numSamples %d numchannels %d",sampleRate,numSamples,numChannels);

        for (int i =0; i < numSamples; ++i) {
            left_audio_buffer.push_back(audioFile.samples[0][i]);
            if (numChannels > 1) {
                right_audio_buffer.push_back(audioFile.samples[1][i]);
            } else {
                right_audio_buffer.push_back(audioFile.samples[0][i]);
            }
        }

        return success;
    }

    void clearBuffers() {
        std::vector<float>().swap(left_audio_buffer);
        std::vector<float>().swap(right_audio_buffer);

        left_audio_buffer.resize(0);
        right_audio_buffer.resize(0);
    }

    audioSample getSample() {

        audioSample samp;
        samp.left = 0;
        samp.right = 0;

        if (currentSample >= numSamples) {
            currentSample = 0;
            INFO("Stopping from inside sample");
        } else {
            samp.left = left_audio_buffer[currentSample];
            samp.right = right_audio_buffer[currentSample];
            currentSample++;
        }


        return samp;
    }


};