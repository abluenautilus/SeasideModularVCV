#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"
#include "../inc/Gate.hpp"
#include "../inc/Glide.hpp"

using namespace daisysp;

#define MAX_TARAFS 13

// // Some parameters are tuned based on 48kHz then scaled accordingly
static const float base_sample_rate = 48000;
static const int base_npt = 1500; // buffer size at 48kHz
static const int max_npt = 10000; // max buffer size

struct Raga : Module {

    enum ParamId {
        STRUMBUTTON_PARAM,
        GLIDESWITCH_PARAM,
        GLIDETYPE_PARAM,
        MAINSTRINGMIX_PARAM,
        JORAMIX_PARAM,
        SECONDMIX_PARAM,
        THIRDMIX_PARAM,
        TARAFMIX_PARAM,
        CHIKARIMIX_PARAM,
        CHIKARIBUTTON_PARAM,
        BUTTON_SA_PARAM,
        BUTTON_REb_PARAM,
        BUTTON_RE_PARAM,
        BUTTON_GAb_PARAM,
        BUTTON_GA_PARAM,
        BUTTON_MA_PARAM,
        BUTTON_MAs_PARAM,
        BUTTON_PA_PARAM,
        BUTTON_DHAb_PARAM,
        BUTTON_DHA_PARAM,
        BUTTON_NIb_PARAM,
        BUTTON_NI_PARAM,
        BUTTON_SA2_PARAM,
        MAIN_TUNING_PARAM,
        SECOND_TUNING_PARAM,
        THIRD_TUNING_PARAM,
        TARAFSTRUMBUTTON_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        STRUM_INPUT,
        PITCH_INPUT,
        CHIKARI_INPUT,
        MAINLEVEL_INPUT,
        JORALEVEL_INPUT,
        SECONDLEVEL_INPUT,
        THIRDLEVEL_INPUT,
        TARAFLEVEL_INPUT,
        CHIKARILEVEL_INPUT,
        MAIN_TUNING_INPUT,
        SECOND_TUNING_INPUT,
        THIRD_TUNING_INPUT,
        TARAFSTRUM_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUTL_OUTPUT,
        AUDIOOUTR_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
        MAINSTRINGMIX_LED,
        JORAMIX_LED,
        SECONDMIX_LED,
        THIRDMIX_LED,
        TARAFMIX_LED,
        CHIKARIMIX_LED,
        BUTTON_SA_LED,
        BUTTON_REb_LED,
        BUTTON_RE_LED,
        BUTTON_GAb_LED,
        BUTTON_GA_LED,
        BUTTON_MA_LED,
        BUTTON_MAs_LED,
        BUTTON_PA_LED,
        BUTTON_DHAb_LED,
        BUTTON_DHA_LED,
        BUTTON_NIb_LED,
        BUTTON_NI_LED,
        BUTTON_SA2_LED,
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;

    // Buttons and triggers
    dsp::SchmittTrigger buttonTrig;
    dsp::SchmittTrigger strumTrig;
    bool strumState, prevStrumState;
    bool tarafStrumState, prevTarafStrumState, tarafStrumButtonState, tarafStrumButtonStatePrev;
    dsp::SchmittTrigger chikariTrig;
    dsp::SchmittTrigger tarafStrumTrig,tarafStrumButtonTrig;
    bool chikariState, prevChikariState;
    bool tarafsEnabled[13];
    bool prevChickButtonState;
    bool prevStrumButtonState;
    bool doStrumNow = false;
    int counter;

    // Strings
    Pluck main_string;
    float main_string_buffer[max_npt];
    float main_string_trig;
    int string_npt = base_npt; 

    Pluck jora_string;
    float jora_string_buffer[max_npt];
    float jora_string_trig;
    int jora_string_npt = base_npt; 

    Pluck second_string;
    float second_string_buffer[max_npt];
    float second_string_trig;
    int second_string_npt = base_npt; 

    Pluck third_string;
    float third_string_buffer[max_npt];
    float third_string_trig;
    int third_string_npt = base_npt; 

    Pluck tarafs[MAX_TARAFS];
    float taraf_buffers[MAX_TARAFS][max_npt];
    float taraf_trigs[MAX_TARAFS];
    int taraf_npts = base_npt;
    AdEnv taraf_envelopes[MAX_TARAFS];
    int downSampleFactor = 2; //we downsamples the tarafs, saves CPU and sounds a bit grittier too

    Pluck chikaris[2];
    float chikari_buffers[2][max_npt];
    float chikari_trig;
    int chikari_npts = base_npt;

    //Comb filters
    Comb main_comb;
    float main_comb_buffer[max_npt];
    int comb_npt = base_npt;

    Comb jora_comb;
    float jora_comb_buffer[max_npt];
    int jora_comb_npt = base_npt;

    Comb second_comb;
    float second_comb_buffer[max_npt];
    int second_comb_npt = base_npt;

    Comb third_comb;
    float third_comb_buffer[max_npt];
    int third_comb_npt = base_npt;

    Comb chikari_comb[2];
    float chikari_comb_buffer[2][max_npt];
    int chikari_comb_npt = base_npt;

    //Effects
    float jawari = 1.0f;
    DcBlock dcblocker;
    Glide glide;
    bool glide_on = true;
    int glide_type,glide_type_prev;

    //Mixing
    float main_string_mix = 0.3;
    float main_comb_mix= 0.7;
    float jora_string_mix = 0.2;
    float jora_comb_mix= 0.8;
    float second_string_mix = 0.8;
    float second_comb_mix= 0.2;
    float third_string_mix = 0.8;
    float third_comb_mix= 0.2;
    float chikari_string_mix = 0.4;
    float chikari_comb_mix = 0.6;

    float mix_main = 0.8;
    float mix_jora = 0.2;
    float mix_second = 0.2;
    float mix_third = 0.2;
    float mix_tarafs = 0.4;
    float mix_chikari = 1;

    //Tuning
    int main_tuning_offset = 0;
    int second_tuning_offset = 0;
    int third_tuning_offset = 0;
    int main_tuning_offset_prev = 0;
    int second_tuning_offset_prev = 0;
    int third_tuning_offset_prev = 0;
    float cv_main_tuning_offset = 0.0;
    float cv_second_tuning_offset = 0.0;
    float cv_third_tuning_offset = 0.0;
    float cv_main_tuning_offset_prev = 0.0;
    float cv_second_tuning_offset_prev = 0.0;
    float cv_third_tuning_offset_prev = 0.0;

    //Notes
    Note mainNote, joraNote, secondNote, thirdNote;
    float cv_pitch, cv_pitch_prev;
    Note taraf_notes[MAX_TARAFS];
    Note chikari_notes[2];
    float current_freq;
    int num_tarafs;

    //Taraf strum items
    Gate tarafStrumGate;
    int tarafStrumCounter;
    float timePerTaraf;
    float tarafStrumDuration = 1.0;
    float taraf_amplifier = 20.0;

    Raga() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(STRUMBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum");
        configInput(STRUM_INPUT, "Strum");
        configInput(PITCH_INPUT,"v/oct main");
        configParam(CHIKARIBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum Chikaris");
        configInput(CHIKARI_INPUT, "Chikari");

        configInput(MAINLEVEL_INPUT, "Main string level");
        configInput(JORALEVEL_INPUT, "Jora string level");
        configInput(SECONDLEVEL_INPUT, "2nd string level");
        configInput(THIRDLEVEL_INPUT, "3rd string level");
        configInput(TARAFLEVEL_INPUT, "Taraf strings level");
        configInput(CHIKARILEVEL_INPUT, "Chikari strings level");
        configInput(MAIN_TUNING_INPUT, "Main Sa tuning");
        configInput(SECOND_TUNING_INPUT, "2nd string tuning");
        configInput(THIRD_TUNING_INPUT, "3rd string tuning");
        configInput(TARAFSTRUM_INPUT, "Taraf string tuning");

        configOutput(AUDIOOUTL_OUTPUT, "Audio Out");

        configParam(MAINSTRINGMIX_PARAM, 0, 1.0f,0.5f, "Main string level");
        configParam(JORAMIX_PARAM, 0, 1.0f,0.5f, "Jora level");
        configParam(SECONDMIX_PARAM, 0, 1.0f,0.5f, "2nd string level");
        configParam(THIRDMIX_PARAM, 0, 1.0f,0.5f, "3rd string level");
        configParam(TARAFMIX_PARAM, 0, 1.0f,0.5f, "Taraf level");
        configParam(CHIKARIMIX_PARAM, 0, 1.0f,0.5f, "Chikari level");

        configParam(GLIDESWITCH_PARAM,0.f,1.f,0.f,"Glide on/off");
        configParam(GLIDETYPE_PARAM,0.f,1.f,0.f,"Glide type lin/log");

        configParam<semitoneKnob>(MAIN_TUNING_PARAM, 1, 11, 1, "Main Sa Tuning");
        paramQuantities[MAIN_TUNING_PARAM]->snapEnabled = true;

        configParam<IndianNoteKnob>(SECOND_TUNING_PARAM, 1, 11, 8, "Second String Tuning");
        paramQuantities[SECOND_TUNING_PARAM]->snapEnabled = true;

        configParam<IndianNoteKnob>(THIRD_TUNING_PARAM, 1, 11, 8, "Third String Tuning");
        paramQuantities[THIRD_TUNING_PARAM]->snapEnabled = true;

        configParam(BUTTON_SA_PARAM, 0.f, 1.f, 0.f, "Sa taraf toggle");
        configParam(BUTTON_REb_PARAM, 0.f, 1.f, 0.f, "Re♭ taraf toggle");
        configParam(BUTTON_RE_PARAM, 0.f, 1.f, 0.f, "Re taraf toggle");
        configParam(BUTTON_GAb_PARAM, 0.f, 1.f, 0.f, "Ga♭ taraf toggle");
        configParam(BUTTON_GA_PARAM, 0.f, 1.f, 0.f, "Ga taraf toggle");
        configParam(BUTTON_MA_PARAM, 0.f, 1.f, 0.f, "Ma taraf toggle");
        configParam(BUTTON_MAs_PARAM, 0.f, 1.f, 0.f, "Ma# taraf toggle");
        configParam(BUTTON_PA_PARAM, 0.f, 1.f, 0.f, "Pa taraf toggle");
        configParam(BUTTON_DHAb_PARAM, 0.f, 1.f, 0.f, "Dha♭ taraf toggle");
        configParam(BUTTON_DHA_PARAM, 0.f, 1.f, 0.f, "Dha taraf toggle");
        configParam(BUTTON_NIb_PARAM, 0.f, 1.f, 0.f, "Ni♭ taraf toggle");
        configParam(BUTTON_NI_PARAM, 0.f, 1.f, 0.f, "Ni taraf toggle");
        configParam(BUTTON_SA2_PARAM, 0.f, 1.f, 0.f, "high Sa taraf toggle");
        configParam(TARAFSTRUMBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum tarafs");
            
        // Make sure buffers are clear
        for (int i = 0; i < max_npt; ++i) {
            main_string_buffer[i] = 0.0f;
            main_comb_buffer[i] = 0.0f;
            jora_string_buffer[i] = 0.0f;
            jora_comb_buffer[i] = 0.0f;
            second_string_buffer[i] = 0.0f;
            second_comb_buffer[i] = 0.0f;
            third_string_buffer[i] = 0.0f;
            third_comb_buffer[i] = 0.0f;
            chikari_buffers[0][i] = 0.0f;
            chikari_buffers[1][i] = 0.0f;
            chikari_comb_buffer[0][i] = 0.0f;
            chikari_comb_buffer[1][i] = 0.0f;
        }

        num_tarafs = 13;
        sampleRate = APP->engine->getSampleRate();

        tarafStrumGate.Init(sampleRate);
        tarafStrumGate.SetDuration(tarafStrumDuration);
        tarafStrumCounter = 0;
        timePerTaraf = tarafStrumDuration / (num_tarafs+1);

        for (int x = 0; x < num_tarafs; ++x) {
            for (int i = 0; i < max_npt; ++i) {
                taraf_buffers[x][i] = 0.0f;
            }
            taraf_envelopes[x].Init(sampleRate);
            taraf_envelopes[x].SetTime(ADENV_SEG_ATTACK,0.2);
            taraf_envelopes[x].SetTime(ADENV_SEG_DECAY,4.0);
            taraf_envelopes[x].SetMin(0.0);
            taraf_envelopes[x].SetMax(1.0);
            
        }

        glide_type = 0;
        glide_type_prev = 0;
        glide.Init(sampleRate,glide_type);
        glide.SetDuration(0.05);

        // Default tuning
        mainNote.setNote("C",3);
        joraNote.setNote("C",3);
        secondNote.setNote("G",3);
        thirdNote.setNote("G",3);

        taraf_notes[0].setNote("C",3);
        taraf_notes[1].setNote("C#",3);
        taraf_notes[2].setNote("D",3);
        taraf_notes[3].setNote("Eb",3);
        taraf_notes[4].setNote("E",3);
        taraf_notes[5].setNote("F",3);
        taraf_notes[6].setNote("Gb",3);
        taraf_notes[7].setNote("G",3);
        taraf_notes[8].setNote("Ab",3);
        taraf_notes[9].setNote("A",3);
        taraf_notes[10].setNote("Bb",3);
        taraf_notes[11].setNote("B",3);
        taraf_notes[12].setNote("C",4);

        chikari_notes[0].setNote("C",4);
        chikari_notes[1].setNote("C",5);

        // Adjust buffers based on sample rate
        float srRatio = sampleRate/base_sample_rate;
        string_npt = clamp(static_cast<int>(base_npt * srRatio),0,max_npt);
        comb_npt = string_npt;

        // Initialize oscillators
        main_string.Init(sampleRate,main_string_buffer,string_npt,daisysp::PLUCK_MODE_RECURSIVE);
        main_string.SetAmp(1.0f);
        main_string.SetDecay(1.0f);
        main_string.SetDamp(0.95f);
        main_string.SetFreq(mainNote.frequency);
        main_string_trig = 0.f;
        main_comb.Init(sampleRate,main_comb_buffer,comb_npt);
        main_comb.SetFreq(mainNote.frequency);

        jora_string.Init(sampleRate,jora_string_buffer,jora_string_npt,daisysp::PLUCK_MODE_RECURSIVE);
        jora_string.SetAmp(1.0f);
        jora_string.SetDecay(1.0f);
        jora_string.SetDamp(0.96f);
        jora_string.SetFreq(joraNote.frequency);
        jora_string_trig = 0.f;
        jora_comb.Init(sampleRate,jora_comb_buffer,jora_comb_npt);
        jora_comb.SetFreq(joraNote.frequency);

        second_string.Init(sampleRate,second_string_buffer,second_string_npt,daisysp::PLUCK_MODE_RECURSIVE);
        second_string.SetAmp(1.0f);
        second_string.SetDecay(1.0f);
        second_string.SetDamp(0.96f);
        second_string.SetFreq(secondNote.frequency);
        second_string_trig = 0.f;
        second_comb.Init(sampleRate,second_comb_buffer,second_comb_npt);
        second_comb.SetFreq(secondNote.frequency);

        third_string.Init(sampleRate,third_string_buffer,third_string_npt,daisysp::PLUCK_MODE_RECURSIVE);
        third_string.SetAmp(1.0f);
        third_string.SetDecay(1.0f);
        third_string.SetDamp(0.96f);
        third_string.SetFreq(thirdNote.frequency);
        third_string_trig = 0.f;
        third_comb.Init(sampleRate,third_comb_buffer,third_comb_npt);
        third_comb.SetFreq(thirdNote.frequency);

        chikaris[0].Init(sampleRate,chikari_buffers[0],chikari_npts,daisysp::PLUCK_MODE_RECURSIVE);
        chikaris[0].SetAmp(1.0f);
        chikaris[0].SetDecay(1.0f);
        chikaris[0].SetDamp(0.98f);
        chikaris[0].SetFreq(chikari_notes[0].frequency);
        
        chikaris[1].Init(sampleRate,chikari_buffers[1],chikari_npts,daisysp::PLUCK_MODE_RECURSIVE);
        chikaris[1].SetAmp(1.0f);
        chikaris[1].SetDecay(1.0f);
        chikaris[1].SetDamp(0.98f);
        chikaris[1].SetFreq(chikari_notes[1].frequency);
        chikari_trig = 0.f;

        chikari_comb[0].Init(sampleRate,chikari_comb_buffer[0],chikari_comb_npt);
        chikari_comb[0].SetFreq(chikari_notes[0].frequency);
        chikari_comb[1].Init(sampleRate,chikari_comb_buffer[1],chikari_comb_npt);
        chikari_comb[1].SetFreq(chikari_notes[1].frequency);

        for (int i = 0; i < num_tarafs; ++i) {
            tarafs[i].Init(sampleRate,taraf_buffers[i],taraf_npts,daisysp::PLUCK_MODE_RECURSIVE);
            tarafs[i].SetAmp(1.0f);
            tarafs[i].SetDecay(1.0f);
            tarafs[i].SetDamp(0.99f);
            tarafs[i].SetFreq(taraf_notes[i].frequency*2);
            taraf_trigs[i] = 0.f;  
        }

        dcblocker.Init(sampleRate);

    }

    json_t *dataToJson() override {

        json_t *root = json_object();

        json_object_set_new(root, "moduleVersion", json_integer(2));
      
        return root;
    }

    void dataFromJson(json_t* root) override {

 

    }

    void doStrum() {

        int cv_main_tuning_semis = round(cv_main_tuning_offset * 12);
        float midi_pitch = (cv_pitch * 12) + 48 + main_tuning_offset + cv_main_tuning_semis;
        float frequency = 440 * pow(2, ((midi_pitch - 69) / 12.0));
        current_freq = frequency;

        main_string.SetFreq(frequency);
        main_comb.SetFreq(frequency);

        glide.SetCurrentState(false);

        // Process tarafs
        for (int i = 0; i < num_tarafs; ++i) {
            if ( abs(current_freq - taraf_notes[i].frequency) < 0.01 ) {
                if (taraf_envelopes[i].GetValue() < 0.2) {
                    //only re-strike the taraf if it's super quiet otherwise we hear the strike
                    taraf_trigs[i] = 1.0;
                }
                taraf_envelopes[i].Trigger();
            }
        }
                            
        main_string_trig = 1.0f;
        jora_string_trig = 1.0f;
        second_string_trig = 1.0f;
        third_string_trig = 1.0f;

    }
    
    void doChikariStrum() {
        
        chikari_trig = 1.0f;

    }

    void doTarafStrum() {
        INFO("Strum tarafs");
        tarafStrumGate.ReTrigger();
    }

    void reTune() {

        int cv_main_tuning_semis = round(cv_main_tuning_offset * 12);
        int cv_second_tuning_semis = round(cv_second_tuning_offset * 12);
        int cv_third_tuning_semis = round(cv_third_tuning_offset * 12);

        float midi_pitch = (cv_pitch * 12) + 48 + main_tuning_offset + cv_main_tuning_semis;
        float tempfreq = 440 * pow(2, ((midi_pitch - 69) / 12.0));
        current_freq = tempfreq;
        main_string.SetFreq(tempfreq);
        main_comb.SetFreq(tempfreq);

        tempfreq = 440 * pow(2, ((joraNote.noteNumMIDI + main_tuning_offset + cv_main_tuning_semis - 69) / 12.0));
        jora_string.SetFreq(tempfreq);
        jora_comb.SetFreq(tempfreq);

        tempfreq = 440 * pow(2, ((joraNote.noteNumMIDI + main_tuning_offset + second_tuning_offset + cv_second_tuning_semis - 69) / 12.0));
        second_string.SetFreq(tempfreq);
        second_comb.SetFreq(tempfreq);

        tempfreq = 440 * pow(2, ((joraNote.noteNumMIDI + main_tuning_offset + third_tuning_offset + cv_third_tuning_semis - 69) / 12.0));
        third_string.SetFreq(tempfreq);
        third_comb.SetFreq(tempfreq);

        tempfreq = 440 * pow(2, ((chikari_notes[0].noteNumMIDI + main_tuning_offset + cv_main_tuning_semis - 69) / 12.0));
        chikaris[0].SetFreq(tempfreq);
        chikari_comb[0].SetFreq(tempfreq);

        tempfreq = 440 * pow(2, ((chikari_notes[1].noteNumMIDI + main_tuning_offset + cv_main_tuning_semis - 69) / 12.0));
        chikaris[1].SetFreq(tempfreq);
        chikari_comb[1].SetFreq(tempfreq);
        
        for (int i = 0; i < num_tarafs; ++i) {
            tempfreq = 440 * pow(2, ((taraf_notes[i].noteNumMIDI + main_tuning_offset + cv_main_tuning_semis - 69) / 12.0));
            tarafs[i].SetFreq(tempfreq*2);
        }

    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override {

        // compare to base freq
        sampleRate = e.sampleRate;
        float srRatio = sampleRate/base_sample_rate;
        int npts = clamp(static_cast<int>(base_npt * srRatio),0,max_npt);

        // Empty buffers
        for (int i = 0; i < max_npt; ++i) {
            main_string_buffer[i] = 0.0f;
            main_comb_buffer[i] = 0.0f;
            jora_string_buffer[i] = 0.0f;
            jora_comb_buffer[i] = 0.0f;
            second_string_buffer[i] = 0.0f;
            second_comb_buffer[i] = 0.0f;
            third_string_buffer[i] = 0.0f;
            third_comb_buffer[i] = 0.0f;
            chikari_buffers[0][i] = 0.0f;
            chikari_buffers[1][i] = 0.0f;
            chikari_comb_buffer[0][i] = 0.0f;
            chikari_comb_buffer[1][i] = 0.0f;
        }
        for (int x = 0; x < num_tarafs; ++x) {
            for (int i = 0; i < max_npt; ++i) {
                taraf_buffers[x][i] = 0.0f;
            }
        }

        main_string.setSampleRate(e.sampleRate,npts);
        main_comb.setSampleRate(e.sampleRate,npts);
        jora_string.setSampleRate(e.sampleRate,npts);
        jora_comb.setSampleRate(e.sampleRate,npts);
        second_string.setSampleRate(e.sampleRate,npts);
        second_comb.setSampleRate(e.sampleRate,npts);
        third_string.setSampleRate(e.sampleRate,npts);
        third_comb.setSampleRate(e.sampleRate,npts);
        
        for (int i = 0; i < num_tarafs; ++i) {
            tarafs[i].setSampleRate(e.sampleRate,npts);
        }
 
        sampleRate = e.sampleRate;
        reTune();

    }

    void process(const ProcessArgs& args) override {

        //pitch input
        cv_pitch_prev = cv_pitch;
        cv_pitch = clamp(inputs[PITCH_INPUT].getVoltage(),-1.0,2.0);

        counter++;
        if (counter == 1) {
            doStrum();
        }

        //handle taraf selection buttons
        for (int i = 10; i <= 22; ++i) {
            bool buttonState = params[i].getValue();
            if (buttonState) {
                lights[i-4].setBrightness(0.1);
            } else {
                lights[i-4].setBrightness(0.0);
            }
           
            tarafsEnabled[i-10] = buttonState;
        }

        //handle taraf strum
        bool strumGateStatus = tarafStrumGate.Process();
        if (strumGateStatus) {
            float next = (tarafStrumCounter + 1) * timePerTaraf;
            if (tarafStrumGate.GetElapsed()+0.01 >= next) {
                INFO("Plucking taraf counter %d elapsed %.3f next %.3f",tarafStrumCounter,tarafStrumGate.GetElapsed(),(tarafStrumCounter +2)*timePerTaraf);
                //skip disabled tarafs
                while (!tarafsEnabled[num_tarafs-tarafStrumCounter-1]) {
                    tarafStrumCounter++;
                }
                taraf_trigs[num_tarafs-tarafStrumCounter-1] = 1;
                taraf_envelopes[num_tarafs-tarafStrumCounter-1].Trigger();
                tarafStrumCounter++;
                if (tarafStrumCounter >= num_tarafs) {
                    INFO("Resetting");
                    tarafStrumCounter = 0;
                    tarafStrumGate.SetElapsed(tarafStrumDuration);
                }
            }
        }

        //Tuning Knobs and cv
        main_tuning_offset_prev = main_tuning_offset;
        second_tuning_offset_prev = second_tuning_offset;
        third_tuning_offset_prev = third_tuning_offset;
        main_tuning_offset = params[MAIN_TUNING_PARAM].getValue()-1;
        second_tuning_offset = params[SECOND_TUNING_PARAM].getValue()-1;
        third_tuning_offset = params[THIRD_TUNING_PARAM].getValue()-1;

        cv_main_tuning_offset_prev = cv_main_tuning_offset;
        cv_second_tuning_offset_prev = cv_second_tuning_offset;
        cv_third_tuning_offset_prev = cv_third_tuning_offset;
        cv_main_tuning_offset = clamp(inputs[MAIN_TUNING_INPUT].getVoltage(),-1.0,2.0);
        cv_second_tuning_offset = clamp(inputs[SECOND_TUNING_INPUT].getVoltage(),-1.0,2.0);
        cv_third_tuning_offset = clamp(inputs[THIRD_TUNING_INPUT].getVoltage(),-1.0,2.0);

        if (main_tuning_offset != main_tuning_offset_prev || cv_main_tuning_offset != cv_main_tuning_offset_prev) {
            reTune();
        }
        if (second_tuning_offset != second_tuning_offset_prev || cv_second_tuning_offset != cv_second_tuning_offset_prev) {
          
            int cv_second_tuning_semis = round(cv_second_tuning_offset * 12);
            float tempfreq = 440 * pow(2, ((joraNote.noteNumMIDI + main_tuning_offset + second_tuning_offset + cv_second_tuning_semis - 69) / 12.0));
            second_string.SetFreq(tempfreq);
            second_comb.SetFreq(tempfreq);
        }
        if (third_tuning_offset != third_tuning_offset_prev || cv_third_tuning_offset != cv_third_tuning_offset_prev) {
            int cv_third_tuning_semis = round(cv_third_tuning_offset * 12);
            float tempfreq = 440 * pow(2, ((joraNote.noteNumMIDI + main_tuning_offset + third_tuning_offset + cv_third_tuning_semis - 69) / 12.0));
            third_string.SetFreq(tempfreq);
            third_comb.SetFreq(tempfreq);
        }

        //string mix sliders and cv
        mix_main = clamp(params[MAINSTRINGMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[MAINLEVEL_INPUT].getVoltage(),-10.0,10.0)/10;
        mix_jora = clamp(params[JORAMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[JORALEVEL_INPUT].getVoltage(),-10.0,10.0)/10;
        mix_second = clamp(params[SECONDMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[SECONDLEVEL_INPUT].getVoltage(),-10.0,10.0)/10;
        mix_third = clamp(params[THIRDMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[THIRDLEVEL_INPUT].getVoltage(),-10.0,10.0)/10;
        mix_tarafs = clamp(params[TARAFMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[TARAFLEVEL_INPUT].getVoltage(),-10.0,10.0)/10;
        mix_chikari = clamp(params[CHIKARIMIX_PARAM].getValue(),0.0f,1.0f) + clamp(inputs[CHIKARILEVEL_INPUT].getVoltage(),-10.0,10.0)/10;

        lights[MAINSTRINGMIX_LED].setBrightness(mix_main/2);
        lights[JORAMIX_LED].setBrightness(mix_jora/2);
        lights[SECONDMIX_LED].setBrightness(mix_second/2);
        lights[THIRDMIX_LED].setBrightness(mix_third/2);
        lights[TARAFMIX_LED].setBrightness(mix_tarafs/2);
        lights[CHIKARIMIX_LED].setBrightness(mix_chikari/2);

        // Check glide switch
        glide_on = params[GLIDESWITCH_PARAM].getValue();
        glide_type = params[GLIDETYPE_PARAM].getValue();
        if (glide_type != glide_type_prev) {
            glide.Init(sampleRate,glide_type);
            glide.SetDuration(0.05);
            glide_type_prev = glide_type;
        }

        // Detect chikari trigger
		float currentChikTrig = inputs[CHIKARI_INPUT].getVoltage();
		chikariTrig.process(rescale(currentChikTrig, 0.1f, 2.0f, 0.f, 1.f));
		prevChikariState = chikariState;
		chikariState = chikariTrig.isHigh();
        if (!prevChikariState && chikariState) {
            doChikariStrum();
        }

        //Detect taraf strum trigger
        float currentTarafStrumTig = inputs[TARAFSTRUM_INPUT].getVoltage();
        tarafStrumTrig.process(rescale(currentTarafStrumTig,0.1f,2.0f,0.f,1.f));
        prevTarafStrumState = tarafStrumState;
        tarafStrumState = tarafStrumTrig.isHigh();
        if (!prevTarafStrumState && tarafStrumState) {
            doTarafStrum();
        }

        // Process button presses
        bool strumButtonState = buttonTrig.process(params[STRUMBUTTON_PARAM].getValue());
		if (strumButtonState &! prevStrumButtonState) {
            doStrum();
		}
        prevStrumButtonState = strumButtonState;

        bool chikButtonState = chikariTrig.process(params[CHIKARIBUTTON_PARAM].getValue());
        if (chikButtonState &! prevChickButtonState) {
            doChikariStrum();
		}
        prevChickButtonState = chikButtonState;

        bool tarafStrumButtonState = tarafStrumButtonTrig.process(params[TARAFSTRUMBUTTON_PARAM].getValue());
        if (tarafStrumButtonState &! tarafStrumButtonStatePrev) {
            doTarafStrum();
        }
        tarafStrumButtonStatePrev = tarafStrumButtonState;

        // Detect strum trigger
		float currentStrumTrig = inputs[STRUM_INPUT].getVoltage();
		strumTrig.process(rescale(currentStrumTrig, 0.1f, 2.0f, 0.f, 1.f));
		prevStrumState = strumState;
		strumState = strumTrig.isHigh();
        if (!prevStrumState && strumState) {
            //do strum on next cycle
            counter = 0;
        } else if (!strumState) {
            if ( cv_pitch != cv_pitch_prev) {

                if (glide_on &! glide.GetCurrentState()){

                        float midi_pitch = (cv_pitch * 12) + 48;
                        float frequency = 440 * pow(2, ((midi_pitch - 69) / 12.0));
                        
                        glide.ReTrigger();
                        glide.SetFreqs(main_string.GetFreq(), frequency);
                }
            }
        }

        /////////////////////////////////
        //
        // AUDIO PROCESSING BEGINS HERE
        // 
        /////////////////////////////////

        float currentVoltage = 0.0;
        float mix = 0.0;

        if (glide_on && glide.GetCurrentState()) {
            float frequency = glide.Process();
            current_freq = frequency;
            main_string.SetFreq(frequency);
            main_comb.SetFreq(frequency);
        }

        if (mix_main > 0) {
            float mainStringVoltage = main_string.Process(main_string_trig);
            if (main_string_trig) {
                main_string_trig = 0;
            }
            float mainCombVoltage = main_comb.Process(mainStringVoltage);
            mix = (mainStringVoltage * main_string_mix) + (mainCombVoltage * main_comb_mix);
            mix = mix * mix_main;
            currentVoltage += mix;
        }
        
        if (mix_jora > 0) {
            float joraStringVoltage = jora_string.Process(jora_string_trig);
            if (jora_string_trig) {jora_string_trig = 0;}
            float joraCombVoltage = jora_comb.Process(joraStringVoltage);
            mix = (joraStringVoltage * jora_string_mix) + (joraCombVoltage * jora_comb_mix);
            mix = mix * mix_jora;
            currentVoltage += mix;
        }

        if (mix_second > 0) {
            float secondStringVoltage = second_string.Process(second_string_trig);
            if (second_string_trig) {second_string_trig = 0;}
            float secondCombVoltage = second_comb.Process(secondStringVoltage);
            mix = (secondStringVoltage * second_string_mix) + (secondCombVoltage * second_comb_mix);
            mix = mix * mix_second;
            currentVoltage += mix;
        }

        if (mix_third > 0) {
            float thirdStringVoltage = third_string.Process(third_string_trig);
            if (third_string_trig) {third_string_trig = 0;}
            float thirdCombVoltage = third_comb.Process(thirdStringVoltage);
            mix = (thirdStringVoltage * third_string_mix) + (thirdCombVoltage * third_comb_mix);
            mix = mix * mix_third;
            currentVoltage += mix;
        }

        if (mix_chikari > 0 ) {
            float chikari1Voltage = chikaris[0].Process(chikari_trig);
            float chikari2Voltage = chikaris[1].Process(chikari_trig);
            float chikariCombVoltage = chikari_comb[0].Process(chikari1Voltage) + chikari_comb[1].Process(chikari2Voltage);
            if (chikari_trig) {chikari_trig = 0;}

            mix = ((chikari2Voltage) * chikari_string_mix) + (chikariCombVoltage * chikari_comb_mix);
            mix = mix * mix_chikari * 0.7;
            currentVoltage += mix;
        }

        if (mix_tarafs > 0) {
            // Process tarafs
            for (int i = 0; i < num_tarafs; ++i) {
                if (tarafsEnabled[i] ) {
                    float env = taraf_envelopes[i].Process();
                    bool strumming = tarafStrumGate.GetCurrentState();
                    if (strumming) {env = 1.0;}
                    if (env > 0 ) {
                        if (env > 0.25) {lights[i+6].setBrightness(env);}
                        
                        if  (APP->engine->getFrame() % downSampleFactor == 0) {
                            float voltage = tarafs[i].Process(taraf_trigs[i]);
                            voltage = voltage * env;

                            if (taraf_trigs[i]) {taraf_trigs[i] = 0.0;}
                            voltage = voltage * mix_tarafs * taraf_amplifier;
                            currentVoltage += voltage;
                        }
                    }
                }
            }
        }
        

        float sig = dcblocker.Process(currentVoltage);

        outputs[AUDIOOUTL_OUTPUT].setVoltage(sig);
       
    }
    

};

struct RagaWidget : ModuleWidget {
    
	RagaWidget(Raga* module) {

		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Raga.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.5,  104)), module, Raga::STRUM_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34,  104)), module, Raga::CHIKARI_INPUT));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(21.5, 96)), module, Raga::STRUMBUTTON_PARAM));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(34, 96)), module, Raga::CHIKARIBUTTON_PARAM));
        
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.5,  104)), module, Raga::PITCH_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.5,  104)), module, Raga::AUDIOOUTL_OUTPUT));

        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(27, 39.5)), module, Raga::MAINSTRINGMIX_PARAM,Raga::MAINSTRINGMIX_LED));  
        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(37, 39.5)), module, Raga::JORAMIX_PARAM,Raga::JORAMIX_LED));
        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(47, 39.5)), module, Raga::SECONDMIX_PARAM,Raga::SECONDMIX_LED));
        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(57, 39.5)), module, Raga::THIRDMIX_PARAM,Raga::THIRDMIX_LED));
        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(67, 39.5)), module, Raga::TARAFMIX_PARAM,Raga::TARAFMIX_LED));    
        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(77, 39.5)), module, Raga::CHIKARIMIX_PARAM,Raga::CHIKARIMIX_LED)); 

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(27, 58)), module, Raga::MAINLEVEL_INPUT)); 
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37, 58)), module, Raga::JORALEVEL_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(47, 58)), module, Raga::SECONDLEVEL_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57, 58)), module, Raga::THIRDLEVEL_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67, 58)), module, Raga::TARAFLEVEL_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(77, 58)), module, Raga::CHIKARILEVEL_INPUT));

        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(21, 86)), module, Raga::BUTTON_SA_PARAM, Raga::BUTTON_SA_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(30, 86)), module, Raga::BUTTON_RE_PARAM, Raga::BUTTON_RE_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(39, 86)), module, Raga::BUTTON_GA_PARAM, Raga::BUTTON_GA_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(48, 86)), module, Raga::BUTTON_MA_PARAM, Raga::BUTTON_MA_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(57, 86)), module, Raga::BUTTON_PA_PARAM, Raga::BUTTON_PA_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(66, 86)), module, Raga::BUTTON_DHA_PARAM, Raga::BUTTON_DHA_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(75, 86)), module, Raga::BUTTON_NI_PARAM, Raga::BUTTON_NI_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(84, 86)), module, Raga::BUTTON_SA2_PARAM, Raga::BUTTON_SA2_LED));

        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(25.5, 79)), module, Raga::BUTTON_REb_PARAM, Raga::BUTTON_REb_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(34.5, 79)), module, Raga::BUTTON_GAb_PARAM, Raga::BUTTON_GAb_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(52.5, 79)), module, Raga::BUTTON_MAs_PARAM, Raga::BUTTON_MAs_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(61.5, 79)), module, Raga::BUTTON_DHAb_PARAM, Raga::BUTTON_DHAb_LED));
        addParam(createLightParamCentered<SeasideLightLatch<LargeSimpleLight<BlueLight>>>(mm2px(Vec(70.5, 79)), module, Raga::BUTTON_NIb_PARAM, Raga::BUTTON_NIb_LED));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100, 35)), module, Raga::MAIN_TUNING_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100, 50)), module, Raga::SECOND_TUNING_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100, 65)), module, Raga::THIRD_TUNING_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110, 35)), module, Raga::MAIN_TUNING_INPUT)); 
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110, 50)), module, Raga::SECOND_TUNING_INPUT)); 
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110, 65)), module, Raga::THIRD_TUNING_INPUT)); 

        addParam(createParamCentered<CKSS>(mm2px(Vec(10, 55.2)), module, Raga::GLIDESWITCH_PARAM));

        addParam(createParamCentered<VCVButton>(mm2px(Vec(100, 86.5)), module, Raga::TARAFSTRUMBUTTON_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110,86.5)), module, Raga::TARAFSTRUM_INPUT)); 


    }
};

Model* modelRaga = createModel<Raga, RagaWidget>("Raga");

