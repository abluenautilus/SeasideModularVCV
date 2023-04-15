#include "AudioFile.h"
#include "plugin.hpp"
#include <string>
#include <vector>
#include "../inc/libresample/include/libresample.h"

#define MAX_SAMPLE_RATE 192000

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
    float* outputBuffer;

    Sample(){}

    Sample(std::string filePathInit, float *buff) {
        outputBuffer = buff;
        filePath = filePathInit;
        load();
    }

    int load() {

        clearBuffers();

        int success = 1;

        if (audioFile.load(filePath)) {
            success = 1;
        } else {
            success = 0;
        }

        sampleRate = audioFile.getSampleRate();
        numSamples = audioFile.getNumSamplesPerChannel();
        numChannels = audioFile.getNumChannels();

        float appSampleRate = APP->engine->getSampleRate();

        for (int i =0; i < numSamples; ++i) {
            left_audio_buffer.push_back(audioFile.samples[0][i]);
            if (numChannels > 1) {
                right_audio_buffer.push_back(audioFile.samples[1][i]);
            } else {
                right_audio_buffer.push_back(audioFile.samples[0][i]);
            }
        }

        if (appSampleRate != sampleRate) {
            if (appSampleRate > MAX_SAMPLE_RATE) {
                INFO("Beyond maximum sample rate, will not resample");
                success = 0;
                left_audio_buffer = {0,0,0,0};
                right_audio_buffer = {0,0,0,0};
            } else {
                interpolate(appSampleRate);
            }
        }
        return success;
    }

    void interpolate(float appSampleRate) {

        double rate = appSampleRate/sampleRate;
        void *handle = resample_open(1,rate,rate);
        int bufferUsed;
        int numSamplesOut = ceil(numSamples * rate);

        resample_process(handle, rate, &left_audio_buffer[0], numSamples, 1, &bufferUsed, &outputBuffer[0], numSamplesOut );

        std::vector<float> left_temp(outputBuffer, outputBuffer + numSamplesOut);
        resample_process(handle, rate, &right_audio_buffer[0], numSamples, 1, &bufferUsed, &outputBuffer[0], numSamplesOut );
        resample_close(handle);
        std::vector<float> right_temp(outputBuffer, outputBuffer + numSamplesOut);

        left_audio_buffer.swap(left_temp);
        right_audio_buffer.swap(right_temp);

        numSamples = numSamplesOut;

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
        } else {
            samp.left = left_audio_buffer[currentSample];
            samp.right = right_audio_buffer[currentSample];
            currentSample++;
        }
        return samp;
    }


};