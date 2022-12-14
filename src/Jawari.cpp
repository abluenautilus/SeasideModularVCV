#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/gui.hpp"

using namespace daisysp;

#define NUM_STRINGS 4


struct Jawari : Module {

    enum ParamId {
		POT1_PARAM,
		POT2_PARAM,
        POT3_PARAM,
        POT4_PARAM,
        BUTTON1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        CLOCK_INPUT,
		JAWARI_INPUT,
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


    float sampleRate = APP->engine->getSampleRate();
    
    // Notes
    Note notes[NUM_STRINGS];
    Note notes_orig[NUM_STRINGS];

    int current_note = -1;

    // Main strings (K-S Pluck)
    Pluck strings[NUM_STRINGS];
    float string_trig[NUM_STRINGS];
    float string_buffer[NUM_STRINGS][1000];
    int string_npt = 1000;  
    float string_weight;

    //Comb filters
    Comb combs[NUM_STRINGS];
    float comb_buffer[NUM_STRINGS][1000];
    int comb_npt = 1000;

    //Effects
    Autowah wahs[NUM_STRINGS];
    float wah_amount = 0.85;
    float jawari = 0.0f;

    dsp::SchmittTrigger buttonTrig;
    dsp::SchmittTrigger clockTrig;
    bool clockState, prevClockState;

    //Mixing
    float string_mix = 0.3;
    float comb_mix= 0.2;
    float wah_mix = 0.2;

    //Tuning
    int semiOffset, octaveOffset, string1Offset = 0;
    int semiOffsetPrev, octaveOffsetPrev, string1OffsetPrev = 0;

    bool reverb_on = false;

    Jawari() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(POT1_PARAM, 0, 1.0f,0.5f, "jawari");
		configParam(POT2_PARAM, 0, 11, 0.0, "transpose"," semitones");
        paramQuantities[POT2_PARAM]->snapEnabled = true;
        configParam(POT3_PARAM, -1, 2, 0.0, "octave");
        paramQuantities[POT3_PARAM]->snapEnabled = true;
        configParam<IndianNoteKnob>(POT4_PARAM, 1, 12, 7, "string 1");
        paramQuantities[POT4_PARAM]->snapEnabled = true;

        configInput(JAWARI_INPUT, "Jawari");

        configParam(BUTTON1_PARAM, 0.f, 1.f, 0.f, "Trigger");

        configOutput(AUDIOOUT1_OUTPUT, "Out");

        // Make sure buffers are clear
        for (int i = 0; i < 1000; ++i) {
            for (int n = 0; n < NUM_STRINGS; n++) {
                string_buffer[n][i] = 0.0f;
                comb_buffer[n][i] = 0.0f;
            }
        }

        // Default tuning
        notes[0].setNote("G",2); notes_orig[0].setNote("G",2);
        notes[1].setNote("C",3); notes_orig[1].setNote("C",3);
        notes[2].setNote("C",3); notes_orig[2].setNote("C",3);
        notes[3].setNote("C",2); notes_orig[3].setNote("C",2);

        // Initialize oscillators
        for (int i = 0; i < NUM_STRINGS; ++i ) {
            strings[i].Init(sampleRate,string_buffer[i],string_npt,daisysp::PLUCK_MODE_RECURSIVE);
            strings[i].SetAmp(1.0f);
            strings[i].SetDecay(1.0f);
            strings[i].SetDamp(1.0f);
            strings[i].SetFreq(notes[i].frequency);
            string_trig[i] = 0.f;

            combs[i].Init(sampleRate,comb_buffer[i],comb_npt);
            combs[i].SetFreq(notes[i].frequency);

            wahs[i].Init(sampleRate);
            wahs[i].SetWah(wah_amount);
            wahs[i].SetDryWet(.999);
     
        };

        string_weight = 1.0f/float(NUM_STRINGS);
        

        INFO("String weight: %.2f",string_weight);
    }


    void doStep() {

        current_note = current_note + 1;
        if (current_note >= NUM_STRINGS) {current_note = 0;};

        INFO("Doing step %d note: %s%d midi %d freq %.2f",current_note,notes[current_note].noteName.c_str(),notes[current_note].octave,notes[current_note].noteNumMIDI,notes[current_note].frequency);

        string_trig[current_note] = 1.0f;

    }

	void process(const ProcessArgs& args) override {

        jawari = clamp(params[POT1_PARAM].getValue() + inputs[JAWARI_INPUT].getVoltage(),0.0f,1.0f);
    
        comb_mix = jawari/2;
        wah_amount = jawari/2 + .4999;
        string_mix = 1- jawari;
        
        lights[JAWARI_LED].setBrightness(jawari);

        // Tuning
        semiOffset = params[POT2_PARAM].getValue();
        octaveOffset = params[POT3_PARAM].getValue();
        string1Offset = params[POT4_PARAM].getValue();

        // We only calculate new frequency if values have changed to limit CPU
        if (semiOffset != semiOffsetPrev || octaveOffset != octaveOffsetPrev || string1Offset != string1OffsetPrev) {

            for (int i = 0; i < 4; ++i) {

                int semi,octave = 0;
                if (i == 0 ) {
                    semi = string1Offset + semiOffset;
                } else {
                    semi = notes_orig[i].toneNum + semiOffset;
                }
                octave = notes_orig[i].octave + octaveOffset;

                if (semi > 12) {
                    octave++;
                    semi = semi - 12;
                } else if (semi < 0) {
                    octave--;
                    semi = 12-semi;
                }

                notes[i].setNote(notes[i].numToNote[semi],octave);

                strings[i].SetFreq(notes[i].frequency);
                combs[i].SetFreq(notes[i].frequency);
            }

            semiOffsetPrev = semiOffset;
            octaveOffsetPrev = octaveOffset;
            string1OffsetPrev = string1Offset;

        }

    

        // Process button press
		if (buttonTrig.process(params[BUTTON1_PARAM].getValue())) {
            INFO("Trigger!");
            doStep();
		};

		// Record incoming clock
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

            wahs[i].SetWah(wah_amount);
            float wahVoltage = wahs[i].Process(stringVoltage);
            float wahWeighted = wahVoltage * string_weight;

            float mix = (stringWeighted * string_mix) + (wahWeighted * (1-comb_mix)) + (combWeighted * comb_mix);
            
            currentVoltage += mix;
     
        };
    
        // open jawari makes things louder, so we balance for that
        float sig;
        float compensationFactor = 1.7;
        sig = (compensationFactor - (compensationFactor - 1)*jawari) * currentVoltage;


        outputs[AUDIOOUT1_OUTPUT].setVoltage(sig);

        string_trig[current_note] = 0.0f;

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

        addParam(createLightParamCentered<VCVLightSlider<BlueLight>>(mm2px(Vec(25.2, 43)), module, Jawari::POT1_PARAM,Jawari::JAWARI_LED));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 77)), module, Jawari::POT2_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25, 77)), module, Jawari::POT3_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40, 77)), module, Jawari::POT4_PARAM));

        addParam(createParamCentered<VCVButton>(mm2px(Vec(25.2, 20)), module, Jawari::BUTTON1_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15, 94)), module, Jawari::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37, 94)), module, Jawari::JAWARI_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.8, 108)), module, Jawari::AUDIOOUT1_OUTPUT));


    }
};

Model* modelJawari = createModel<Jawari, JawariWidget>("Jawari");
