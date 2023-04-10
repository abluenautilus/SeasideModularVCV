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
    std::vector<float> left_audio_buffer_temp;
    std::vector<float> right_audio_buffer_temp;
    int numChannels;
    float sampleRate;
    int numSamples;
    AudioFile<float> audioFile;
    int currentSample = 0;

    Sample(){}

    Sample(std::string filePathInit) {
        filePath = filePathInit;
        load();
    }

    int load() {

        clearBuffers();

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

        float appSampleRate = APP->engine->getSampleRate();

        INFO("SampleRate %.2f numSamples %d numchannels %d",sampleRate,numSamples,numChannels);

        for (int i =0; i < numSamples; ++i) {
            left_audio_buffer.push_back(audioFile.samples[0][i]);
            if (numChannels > 1) {
                right_audio_buffer.push_back(audioFile.samples[1][i]);
            } else {
                right_audio_buffer.push_back(audioFile.samples[0][i]);
            }
        }

        if (appSampleRate != sampleRate) {
            INFO("Resampling");
            interpolate(appSampleRate);
        }
        INFO("Done loading sample");
        return success;
    }

    void interpolate(float appSampleRate) {

        std::vector<float>().swap(left_audio_buffer_temp);
        std::vector<float>().swap(right_audio_buffer_temp);
        left_audio_buffer_temp.resize(0);
        right_audio_buffer_temp.resize(0);

        INFO("Starting resample...");
        INFO("Num samples: %d",numSamples);
        double rate = sampleRate/appSampleRate;


        INFO("Rate is %.2f",rate);
        hermite_resample(rate, left_audio_buffer, left_audio_buffer_temp, numSamples/rate);
        hermite_resample(rate, right_audio_buffer, right_audio_buffer_temp, numSamples/rate);

        INFO("Swapping resample...");
        left_audio_buffer.swap(left_audio_buffer_temp);
        right_audio_buffer.swap(right_audio_buffer_temp);

        numSamples =left_audio_buffer.size();

    }

     inline double sample_hermite_4p_3o(double x, double * y)
    {
        static double c0, c1, c2, c3;
        c0 = y[1];
        c1 = (1.0 / 2.0)*(y[2] - y[0]);
        c2 = (y[0] - (5.0 / 2.0)*y[1]) + (2.0*y[2] - (1.0 / 2.0)*y[3]);
        c3 = (1.0 / 2.0)*(y[3] - y[0]) + (3.0 / 2.0)*(y[1] - y[2]);
        return ((c3*x + c2)*x + c1)*x + c0;
    }

    inline void hermite_resample( double rate,  std::vector<float> & input, std::vector<float> & output,  uint32_t samplesToProcess)
    {
 
        double virtualReadIndex = 1;
        double i, sample;
        uint32_t n = samplesToProcess - 1;
        while (n--)
        {
            uint32_t readIndex = static_cast<uint32_t>(virtualReadIndex);
            i = virtualReadIndex - readIndex;
            double samps[4] = { input[readIndex - 1], input[readIndex], input[readIndex + 1], input[readIndex + 2] };
            sample = sample_hermite_4p_3o(i, samps); // cubic hermite interpolate over 4 samples
            output.push_back(static_cast<float>(sample));
            virtualReadIndex += rate;

        }
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