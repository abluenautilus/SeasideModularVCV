#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"
#include "../inc/Gate.hpp"
#include "../inc/Glide.hpp"

using namespace daisysp;

#define NUM_TARAFS 12

// // Some parameters are tuned based on 48kHz then scaled accordingly
static const float base_sample_rate = 48000;
static const int base_npt = 1500; // buffer size at 48kHz
static const int max_npt = 10000; // max buffer size

struct Raga : Module {

    enum ParamId {
        STRUMBUTTON_PARAM,
        GLIDESWITCH_PARAM,
        GLIDETYPE_PARAM,
        JORAMIX_PARAM,
        SECONDMIX_PARAM,
        THIRDMIX_PARAM,
        TARAFMIX_PARAM,
        CHIKARIMIX_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        STRUM_INPUT,
        PITCH_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUTL_OUTPUT,
        AUDIOOUTR_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
        JORAMIX_LED,
        SECONDMIX_LED,
        THIRDMIX_LED,
        TARAFMIX_LED,
        CHIKARIMIX_LED,
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;

    // Buttons and triggers
    dsp::SchmittTrigger buttonTrig;
    dsp::SchmittTrigger strumTrig;
    bool strumState, prevStrumState;

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

    Pluck tarafs[NUM_TARAFS];
    float taraf_buffers[NUM_TARAFS][max_npt];
    float taraf_trigs[NUM_TARAFS];
    int taraf_npts = base_npt;

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


    //Effects
    float jawari = 1.0f;
    DcBlock dcblocker;
    Glide glide;
    bool glide_on = true;
    int glide_type,glide_type_prev;

    //Mixing
    float main_string_mix = 0.5;
    float main_comb_mix= 0.5;
    float jora_string_mix = 0.4;
    float jora_comb_mix= 0.6;
    float second_string_mix = 0.8;
    float second_comb_mix= 0.2;
    float third_string_mix = 0.8;
    float third_comb_mix= 0.2;

    float mix_main = 0.8;
    float mix_jora = 0.2;
    float mix_second = 0.2;
    float mix_third = 0.2;
    float mix_tarafs = 0.4;
    float mix_chikari = 0.5;

    //Notes
    Note mainNote, joraNote, secondNote, thirdNote;
    float cv_pitch, cv_pitch_prev;
    Note taraf_notes[NUM_TARAFS];
    float current_freq;


    Raga() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(STRUMBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum");
        configInput(STRUM_INPUT, "Strum");
        configInput(PITCH_INPUT,"v/oct main");

        configOutput(AUDIOOUTL_OUTPUT, "Out Left");
        configOutput(AUDIOOUTR_OUTPUT, "Out Right");

        configParam(JORAMIX_PARAM, 0, 1.0f,0.5f, "Jora level");
        configParam(SECONDMIX_PARAM, 0, 1.0f,0.5f, "2nd string level");
        configParam(THIRDMIX_PARAM, 0, 1.0f,0.5f, "3rd string level");
        configParam(TARAFMIX_PARAM, 0, 1.0f,0.5f, "Taraf level");
        configParam(CHIKARIMIX_PARAM, 0, 1.0f,0.5f, "Chikari level");

        configParam(GLIDESWITCH_PARAM,0.f,1.f,0.f,"Glide on/off");
        configParam(GLIDETYPE_PARAM,0.f,1.f,0.f,"Glide type lin/log");


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
        }

        for (int x = 0; x < NUM_TARAFS; ++x) {
            for (int i = 0; i < max_npt; ++i) {
                taraf_buffers[x][i] = 0.0f;
            }
        }

        sampleRate = APP->engine->getSampleRate();

        glide_type = 0;
        glide_type_prev = 0;
        glide.Init(sampleRate,glide_type);
        glide.SetDuration(0.05);

        // Default tuning
        mainNote.setNote("C",3);
        joraNote.setNote("C",3);
        secondNote.setNote("G",3);
        thirdNote.setNote("G",3);

        taraf_notes[0].setNote("C",2);
        taraf_notes[1].setNote("C#",2);
        taraf_notes[2].setNote("E",2);
        taraf_notes[3].setNote("F",2);
        taraf_notes[4].setNote("G",2);
        taraf_notes[5].setNote("A",2);
        taraf_notes[6].setNote("B",2);
        taraf_notes[7].setNote("C",3);
        taraf_notes[8].setNote("C#",3);
        taraf_notes[9].setNote("D",3);
        taraf_notes[10].setNote("B",1);
        taraf_notes[11].setNote("G",1);

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

        for (int i = 0; i < NUM_TARAFS; ++i) {
            tarafs[i].Init(sampleRate,taraf_buffers[i],taraf_npts,daisysp::PLUCK_MODE_RECURSIVE);
            tarafs[i].SetAmp(1.0f);
            tarafs[i].SetDecay(1.0f);
            tarafs[i].SetDamp(0.9f);
            tarafs[i].SetFreq(thirdNote.frequency);
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

        INFO("STRUM A: Freq: %.2f cv pitch: %.2f",main_string.GetFreq(),cv_pitch);

        float midi_pitch = (cv_pitch * 12) + 48;
        float frequency = 440 * pow(2, ((midi_pitch - 69) / 12.0));

        main_string.SetFreq(frequency);
        main_comb.SetFreq(frequency);

        current_freq = frequency;

        INFO("STRUM B: Freq: %.2f",main_string.GetFreq());

        glide.SetCurrentState(false);
                            
        main_string_trig = 1.0f;
        jora_string_trig = 1.0f;
        second_string_trig = 1.0f;
        third_string_trig = 1.0f;

    }

    void process(const ProcessArgs& args) override {

        cv_pitch = clamp(inputs[PITCH_INPUT].getVoltage(),-1.0,2.0);

        mix_jora = clamp(params[JORAMIX_PARAM].getValue(),0.0f,1.0f);
        mix_second = clamp(params[SECONDMIX_PARAM].getValue(),0.0f,1.0f);
        mix_third = clamp(params[THIRDMIX_PARAM].getValue(),0.0f,1.0f);
        mix_tarafs = clamp(params[TARAFMIX_PARAM].getValue(),0.0f,1.0f);
        mix_chikari = clamp(params[CHIKARIMIX_PARAM].getValue(),0.0f,1.0f);

        lights[JORAMIX_LED].setBrightness(mix_jora/2);
        lights[SECONDMIX_LED].setBrightness(mix_second/2);
        lights[THIRDMIX_LED].setBrightness(mix_third/2);
        lights[TARAFMIX_LED].setBrightness(mix_tarafs/2);
        lights[CHIKARIMIX_LED].setBrightness(mix_chikari/2);

        // Process button press
		if (buttonTrig.process(params[STRUMBUTTON_PARAM].getValue())) {
            doStrum();
		}

        // Check glide switch
        glide_on = params[GLIDESWITCH_PARAM].getValue();
        glide_type = params[GLIDETYPE_PARAM].getValue();
        if (glide_type != glide_type_prev) {
            glide.Init(sampleRate,glide_type);
            glide.SetDuration(0.05);
            glide_type_prev = glide_type;
        }

        // Detect strum trigger
		float currentStrumTrig = inputs[STRUM_INPUT].getVoltage();
		strumTrig.process(rescale(currentStrumTrig, 0.1f, 2.0f, 0.f, 1.f));
		prevStrumState = strumState;
		strumState = strumTrig.isHigh();
        if (!prevStrumState && strumState) {
            doStrum();
        } else if (!strumState) {
            if ( cv_pitch != cv_pitch_prev) {

                INFO("CV pitch %.2f cv pitch prev %.2f strumState %d",cv_pitch,cv_pitch_prev,strumState);

                if (glide_on &! glide.GetCurrentState()){

                        float midi_pitch = (cv_pitch * 12) + 48;
                        float frequency = 440 * pow(2, ((midi_pitch - 69) / 12.0));
                        
                        glide.ReTrigger();
                        glide.SetFreqs(main_string.GetFreq(), frequency);
                        INFO("Starting glide from %.2f to %.2f",main_string.GetFreq(),frequency);

                }

            }
        }

        float currentVoltage = 0;
        
        if (glide_on && glide.GetCurrentState()) {
            float frequency = glide.Process();
            current_freq = frequency;
            main_string.SetFreq(frequency);
            main_comb.SetFreq(frequency);
        }

        float mainStringVoltage = main_string.Process(main_string_trig);
        if (main_string_trig) {main_string_trig = 0;}
        float mainCombVoltage = main_comb.Process(mainStringVoltage);
        float mix = (mainStringVoltage * main_string_mix) + (mainCombVoltage * main_comb_mix);
        mix = mix * mix_main;
        currentVoltage += mix;

        float joraStringVoltage = jora_string.Process(jora_string_trig);
        if (jora_string_trig) {jora_string_trig = 0;}
        float joraCombVoltage = jora_comb.Process(joraStringVoltage);
        mix = (joraStringVoltage * jora_string_mix) + (joraCombVoltage * jora_comb_mix);
        mix = mix * mix_jora;
        currentVoltage += mix;

        float secondStringVoltage = second_string.Process(second_string_trig);
        if (second_string_trig) {second_string_trig = 0;}
        float secondCombVoltage = second_comb.Process(secondStringVoltage);
        mix = (secondStringVoltage * second_string_mix) + (secondCombVoltage * second_comb_mix);
        mix = mix * mix_second;
        currentVoltage += mix;

        float thirdStringVoltage = third_string.Process(third_string_trig);
        if (third_string_trig) {third_string_trig = 0;}
        float thirdCombVoltage = third_comb.Process(thirdStringVoltage);
        mix = (thirdStringVoltage * third_string_mix) + (thirdCombVoltage * third_comb_mix);
        mix = mix * mix_third;
        currentVoltage += mix;

        // Process tarafs
        for (int i = 0; i < NUM_TARAFS; ++i) {

            

        }


        float sig = dcblocker.Process(currentVoltage);

        outputs[AUDIOOUTL_OUTPUT].setVoltage(sig);
        outputs[AUDIOOUTR_OUTPUT].setVoltage(sig);
       
        cv_pitch_prev = cv_pitch;

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

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13,  97)), module, Raga::STRUM_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.7,  97)), module, Raga::PITCH_INPUT));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(13, 113)), module, Raga::STRUMBUTTON_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(75.3,  97)), module, Raga::AUDIOOUTL_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(88.5,  97)), module, Raga::AUDIOOUTR_OUTPUT));

        addParam(createLightParamCentered<BigLightSlider<SeasideBlueLight>>(mm2px(Vec(23, 39.5)), module, Raga::JORAMIX_PARAM,Raga::JORAMIX_LED));
        addParam(createLightParamCentered<BigLightSlider<SeasideBlueLight>>(mm2px(Vec(33, 39.5)), module, Raga::SECONDMIX_PARAM,Raga::SECONDMIX_LED));
        addParam(createLightParamCentered<BigLightSlider<SeasideBlueLight>>(mm2px(Vec(43, 39.5)), module, Raga::THIRDMIX_PARAM,Raga::THIRDMIX_LED));
        addParam(createLightParamCentered<BigLightSlider<SeasideBlueLight>>(mm2px(Vec(53, 39.5)), module, Raga::TARAFMIX_PARAM,Raga::TARAFMIX_LED));    
        addParam(createLightParamCentered<BigLightSlider<SeasideBlueLight>>(mm2px(Vec(63, 39.5)), module, Raga::CHIKARIMIX_PARAM,Raga::CHIKARIMIX_LED));  

        addParam(createParamCentered<CKSS>(mm2px(Vec(9, 41)), module, Raga::GLIDESWITCH_PARAM));
        addParam(createParamCentered<CKSS>(mm2px(Vec(9, 55)), module, Raga::GLIDETYPE_PARAM));

    }
};

Model* modelRaga = createModel<Raga, RagaWidget>("Raga");

