#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"

using namespace daisysp;

#define NUM_STRINGS 4

// // Some parameters are tuned based on 48kHz then scaled accordingly
static const float base_sample_rate = 48000;
static const int base_npt = 1500; // buffer size at 48kHz
static const int max_npt = 10000; // max buffer size

struct Jawari : Module {

    enum ParamId {
		JAWARI_PARAM,
		SEMI_PARAM,
        OCTAVE_PARAM,
        TUNING_PARAM,
        STRUMBUTTON_PARAM,
        RESETBUTTON_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        CLOCK_INPUT,
		JAWARI_INPUT,
        RESET_INPUT,
        PITCH_INPUT,
        STRING_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUT1_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
        JAWARI_LED,
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;
    
    // Notes
    Note notes[NUM_STRINGS];
    Note notes_orig[NUM_STRINGS];

    int current_note = -1;

    // Main strings (K-S Pluck)
    Pluck strings[NUM_STRINGS];
    float string_trig[NUM_STRINGS];
    float string_buffer[NUM_STRINGS][max_npt];
    int string_npt = base_npt;  
    float string_weight;

    //Comb filters
    Comb combs[NUM_STRINGS];
    float comb_buffer[NUM_STRINGS][max_npt];
    int comb_npt = base_npt;

    //Effects
    float jawari = 0.0f;
    DcBlock dcblocker;

    dsp::SchmittTrigger buttonTrig;
    dsp::SchmittTrigger resetbuttonTrig;
    dsp::SchmittTrigger clockTrig;
    dsp::SchmittTrigger resetTrig;
    bool clockState, prevClockState;
    bool resetState, prevResetState;

    //Mixing
    float string_mix = 0.3;
    float comb_mix= 0.2;

    //Tuning
    int semiOffset, octaveOffset, string1Offset = 0;
    int semiOffsetPrev, octaveOffsetPrev, string1OffsetPrev = 0;
    float cv_pitch, cv_string, cv_pitch_prev, cv_string_prev = 0.0;
    int cv_string_octaves = 0;
    int cv_string_semis = 0 ;
    int cv_string_total_semis = 0;
    int cv_pitch_octaves = 0;
    int cv_pitch_semis = 0;

    Jawari() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(JAWARI_PARAM, 0, 1.0f,0.5f, "Bridge shape");
		configParam(SEMI_PARAM, 0, 11, 0.0, "transpose"," semitones");
        paramQuantities[SEMI_PARAM]->snapEnabled = true;
        configParam(OCTAVE_PARAM, 0.0, 2, 0.0, "transpose"," octaves");
        paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
        configParam<IndianNoteKnob>(TUNING_PARAM, 1, 12, 8, "tuning (first string)");
        paramQuantities[TUNING_PARAM]->snapEnabled = true;

        configInput(JAWARI_INPUT, "Bridge shape");
        configInput(CLOCK_INPUT, "Strum (clock input)");
        configInput(RESET_INPUT, "Reset");
        configInput(PITCH_INPUT,"v/oct main");
        configInput(STRING_INPUT,"v/oct 1st string");

        configParam(STRUMBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum");
        configParam(RESETBUTTON_PARAM, 0.f, 1.f, 0.f, "Reset");
        configOutput(AUDIOOUT1_OUTPUT, "Out");

        // Make sure buffers are clear
        for (int i = 0; i < max_npt; ++i) {
            for (int n = 0; n < NUM_STRINGS; n++) {
                string_buffer[n][i] = 0.0f;
                comb_buffer[n][i] = 0.0f;
            }
        }

        sampleRate = APP->engine->getSampleRate();

        // Default tuning
        notes[0].setNote("G",2); notes_orig[0].setNote("G",2);
        notes[1].setNote("C",3); notes_orig[1].setNote("C",3);
        notes[2].setNote("C",3); notes_orig[2].setNote("C",3);
        notes[3].setNote("C",2); notes_orig[3].setNote("C",2);

        float srRatio = sampleRate/base_sample_rate;
        string_npt = clamp(static_cast<int>(base_npt * srRatio),0,max_npt);
        comb_npt = string_npt;

        // Initialize oscillators
        for (int i = 0; i < NUM_STRINGS; ++i ) {
            strings[i].Init(sampleRate,string_buffer[i],string_npt,daisysp::PLUCK_MODE_RECURSIVE);
            strings[i].SetAmp(1.0f);
            strings[i].SetDecay(1.0f);
            strings[i].SetDamp(0.995f);
            strings[i].SetFreq(notes[i].frequency);
            string_trig[i] = 0.f;

            combs[i].Init(sampleRate,comb_buffer[i],comb_npt);
            combs[i].SetFreq(notes[i].frequency);

        };

        dcblocker.Init(sampleRate);
        string_weight = 1.0f/float(NUM_STRINGS);

    }


    void doStep() {

        current_note = current_note + 1;
        if (current_note >= NUM_STRINGS) {current_note = 0;};
        string_trig[current_note] = 1.0f;

        // Tuning
        semiOffset = params[SEMI_PARAM].getValue();
        octaveOffset = params[OCTAVE_PARAM].getValue();
        string1Offset = params[TUNING_PARAM].getValue();
        cv_pitch = clamp(inputs[PITCH_INPUT].getVoltage(),-1.0,2.0);
        cv_string = clamp(inputs[STRING_INPUT].getVoltage(),-1.0,2.0);

        // We only calculate new frequency if values have changed to limit CPU
        if ( string1Offset != string1OffsetPrev || cv_string != cv_string_prev) {
            // First string tuning has changed

            cv_string_octaves = floor(cv_string);
            cv_string_semis = static_cast<int>((cv_string - cv_string_octaves)*12);
            cv_string_total_semis = 12 * cv_string_octaves + cv_string_semis;

            int semi,octave = 0;
            semi = string1Offset + semiOffset + cv_string_total_semis;
            octave = notes_orig[0].octave + octaveOffset + cv_pitch_octaves;

            if (semi > 12) {
                octave++;
                semi = semi - 12;
            } else if (semi < 0) {
                octave--;
                semi = 12-semi;
            }
            semi = semi + cv_pitch_semis;

            // Retune first string
            notes[0].setNote(notes[0].numToNote[semi],octave);
            strings[0].SetFreq(notes[0].frequency );
            combs[0].SetFreq(notes[0].frequency );

      
            cv_string_prev = cv_string;
            string1OffsetPrev = string1Offset;
        }

        if (semiOffset != semiOffsetPrev || octaveOffset != octaveOffsetPrev || cv_pitch != cv_pitch_prev) {

            cv_pitch_octaves = floor(cv_pitch);
            cv_pitch_semis = static_cast<int>((cv_pitch - cv_pitch_octaves)*12);

            for (int i = 1; i < 4; ++i) {

                int semi,octave = 0;
                semi = notes_orig[i].toneNum + semiOffset;
                octave = notes_orig[i].octave + octaveOffset + cv_pitch_octaves;

                if (semi > 12) {
                    octave++;
                    semi = semi - 12;
                } else if (semi < 0) {
                    octave--;
                    semi = 12-semi;
                }

                semi = semi + cv_pitch_semis;

                // Retune string
                notes[i].setNote(notes[i].numToNote[semi],octave);
                strings[i].SetFreq(notes[i].frequency );
                combs[i].SetFreq(notes[i].frequency );
            }

            semiOffsetPrev = semiOffset;
            octaveOffsetPrev = octaveOffset;
            cv_pitch_prev = cv_pitch;

        }

    }


    void onSampleRateChange(const SampleRateChangeEvent& e) override {

        // compare to base freq
        sampleRate = e.sampleRate;
        float srRatio = sampleRate/base_sample_rate;
        int npts = clamp(static_cast<int>(base_npt * srRatio),0,max_npt);

        // Empty buffers
        for (int i = 0; i < max_npt; ++i) {
            for (int n = 0; n < NUM_STRINGS; n++) {
                string_buffer[n][i] = 0.0f;
                comb_buffer[n][i] = 0.0f;
            }
        }
        for (int i = 0; i < NUM_STRINGS; i++) {
            strings[i].setSampleRate(e.sampleRate,npts);
            combs[i].setSampleRate(e.sampleRate,npts);
            strings[i].SetFreq(notes[i].frequency );
            combs[i].SetFreq(notes[i].frequency );

        }

        sampleRate = e.sampleRate;

    }

	void process(const ProcessArgs& args) override {

        jawari = clamp(params[JAWARI_PARAM].getValue() + inputs[JAWARI_INPUT].getVoltage(),0.0f,1.0f);
        comb_mix = jawari * 2.5;
        string_mix = 2.5 - comb_mix;
        lights[JAWARI_LED].setBrightness(jawari);

        // Process button press
		if (buttonTrig.process(params[STRUMBUTTON_PARAM].getValue())) {
            doStep();
		}

		// Detect incoming clock
		float currentClockTrig = inputs[CLOCK_INPUT].getVoltage();
		clockTrig.process(rescale(currentClockTrig, 0.1f, 2.0f, 0.f, 1.f));
		prevClockState = clockState;
		clockState = clockTrig.isHigh();
        if (!prevClockState && clockState) {
            doStep();
        }

        float currentVoltage = 0;
  
        for (int i = 0; i < NUM_STRINGS; ++i ) {

            float stringVoltage = strings[i].Process(string_trig[i]);
            float stringWeighted = stringVoltage * string_weight;
            if (string_trig[1]) {string_trig[i] = 0;}

            float combVoltage = combs[i].Process(stringVoltage);
            float combWeighted = combVoltage * string_weight;

            float mix = (stringWeighted * string_mix) + (combWeighted * comb_mix);
            
            currentVoltage += mix;
     
        };

        //jawari makes things louder, so we balance for that
        float sig;
        float compensationFactor = 5;
        float multiplier = compensationFactor - (compensationFactor - jawari)*jawari;
        sig = multiplier * currentVoltage;

        sig = dcblocker.Process(sig);

        outputs[AUDIOOUT1_OUTPUT].setVoltage(sig);
        string_trig[current_note] = 0.0f;

        // Detect reset
		float currentResetTrig = inputs[RESET_INPUT].getVoltage();
		resetTrig.process(rescale(currentResetTrig, 0.1f, 2.0f, 0.f, 1.f));
		prevResetState = resetState;
		resetState = resetTrig.isHigh();
        if (!prevResetState && resetState) {
            current_note = -1;
        }
        if (resetbuttonTrig.process(params[RESETBUTTON_PARAM].getValue())) {
            current_note = -1;
		}


    }
};


struct JawariWidget : ModuleWidget {
	JawariWidget(Jawari* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Jawari.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createLightParamCentered<RubberLightSlider<SeasideBlueLight>>(mm2px(Vec(25.4, 39.5)), module, Jawari::JAWARI_PARAM,Jawari::JAWARI_LED));
        
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.8, 88)), module, Jawari::SEMI_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.4, 88)), module, Jawari::OCTAVE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42, 88)), module, Jawari::TUNING_PARAM));

        addParam(createParamCentered<VCVButton>(mm2px(Vec(25.4, 113)), module, Jawari::STRUMBUTTON_PARAM));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(8.8, 113)), module, Jawari::RESETBUTTON_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 67)), module, Jawari::JAWARI_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4, 105)), module, Jawari::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.8, 105)), module, Jawari::RESET_INPUT));
        addInput(createInputCentered<PJ301MPortBlue>(mm2px(Vec(8.8, 55)), module, Jawari::PITCH_INPUT));
        addInput(createInputCentered<PJ301MPortBlue>(mm2px(Vec(42, 55)), module, Jawari::STRING_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(42, 105)), module, Jawari::AUDIOOUT1_OUTPUT));


    }
};

Model* modelJawari = createModel<Jawari, JawariWidget>("Jawari");