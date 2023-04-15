#include "plugin.hpp"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"
#include "../inc/Bol.hpp"
#include "../inc/Thekas.hpp"
#include "../inc/Gate.hpp"

#define NUM_BOLS 14
#define NUM_BOL_BUTTONS 12

struct Tala : Module {

    enum ParamId {
        BUTTON1_PARAM,
        BUTTON2_PARAM,
        BUTTON3_PARAM,
        BUTTON4_PARAM,
        BUTTON5_PARAM,
        BUTTON6_PARAM,
        BUTTON7_PARAM,
        BUTTON8_PARAM,
        BUTTON9_PARAM,
        BUTTON10_PARAM,
        BUTTON11_PARAM,
        BUTTON12_PARAM,
        BUTTONUP_PARAM,
        BUTTONDOWN_PARAM,
        MODESWITCH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        INPUT1,
        INPUT2,
        INPUT3,
        INPUT4,
        INPUT5,
        INPUT6,
        INPUT7,
        INPUT8,
        INPUT9,
        INPUT10,
        INPUT11,
        INPUT12,
        CLOCK_INPUT,
        RESET_INPUT,
        ACC_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUTL_OUTPUT,
        AUDIOOUTR_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
        LIGHT1,
        LIGHT2,
        LIGHT3,
        LIGHT4,
        LIGHT5,
        LIGHT6,
        LIGHT7,
        LIGHT8,
        LIGHT9,
        LIGHT10,
        LIGHT11,
        LIGHT12,
        SAM_LED,
        ACC_LED,
        PLAYING_LED,
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;
    bool isPlaying , silentMode = false;
    bool silentModePrev = false;

	dsp::SchmittTrigger buttonTrig[12];
    dsp::SchmittTrigger inputTrig[12];
    dsp::SchmittTrigger upButtonTrig;
    dsp::SchmittTrigger downButtonTrig;
    dsp::SchmittTrigger resetTrig;
    dsp::SchmittTrigger clockTrig;
    dsp::SchmittTrigger accTrig;

    float previousTriggers[12];
    float prevResetTrig, prevClockTrig, prevAccTrig;

    Bol bols[NUM_BOLS];
    int mode;
    float gain = 6.0;

    int currentTheka = 0; 
    int currentStep = 0;
    int accent = 0;
    float accentfactor= 2;

    Gate accentGate, silentGate, fadeOutGate, fadeInGate;
    Gate lightGates[NUM_BOL_BUTTONS];

    bool prevAccent = false;
    bool fadingOut,fadingIn,fadingOutPrev,fadingInPrev;
    bool waiting, waitingPrev = false;

    //bpm estimation
    float currentFrame,trigToTrigTime,prevFrame,triggerGapAccumulator, triggerGapAverage, triggerGapSeconds;
    int numRecentTriggers, numTotalTriggers;
    int numTriggersToAverage = 6;

    ThekaLibrary thekalib;

    // this has to be big enough to hold upsampled samples
    // longest sample is 2 seconds
    // 2 * 60 * 192000 = 2304000
    float interpolationBuffer[2304000]; 

    Tala() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(MODESWITCH_PARAM, 0.f, 2.f, 0.f, "Sample Mode");
        configParam(BUTTON1_PARAM, 0.f, 1.f, 0.f,"Dha");
        configParam(BUTTON2_PARAM, 0.f, 1.f, 0.f,"Dhin");
        configParam(BUTTON3_PARAM, 0.f, 1.f, 0.f,"Dhit");
        configParam(BUTTON4_PARAM, 0.f, 1.f, 0.f,"Dhun");
        configParam(BUTTON5_PARAM, 0.f, 1.f, 0.f,"Ga");
        configParam(BUTTON6_PARAM, 0.f, 1.f, 0.f,"Ge");
        configParam(BUTTON7_PARAM, 0.f, 1.f, 0.f,"Ke");
        configParam(BUTTON8_PARAM, 0.f, 1.f, 0.f,"Na");
        configParam(BUTTON9_PARAM, 0.f, 1.f, 0.f,"Ta");
        configParam(BUTTON10_PARAM, 0.f, 1.f, 0.f,"Ti");
        configParam(BUTTON11_PARAM, 0.f, 1.f, 0.f,"Tin");
        configParam(BUTTON12_PARAM, 0.f, 1.f, 0.f,"Tun");
        configParam(BUTTONUP_PARAM, 0.f, 1.f, 0.f,"Prev Theka");
        configParam(BUTTONDOWN_PARAM, 0.f, 1.f, 0.f,"Next Theka");

        configOutput(AUDIOOUTL_OUTPUT, "Out L");
        configOutput(AUDIOOUTR_OUTPUT, "Out R");

        mode = 0; // 0 = first sample only, 1 = round robin, 2 = random

        // //SET UP bols
        for (int i = 0; i < NUM_BOLS; ++i) {
            bols[i] = Bol(BOLS[i], interpolationBuffer);
            bols[i].mode = mode;
        }

        sampleRate = APP->engine->getSampleRate();

        for (int i = 0; i < NUM_BOL_BUTTONS; ++ i) {
            lightGates[i].Init(sampleRate);
            lightGates[i].SetDuration(0.25);
        }

        accentGate.Init(sampleRate);
        accentGate.SetDuration(0.125);

    }

    json_t *dataToJson() override {

		json_t *root = json_object();

		json_object_set_new(root, "moduleVersion", json_integer(2));
		json_object_set_new(root, "currentTheka", json_integer(currentTheka));

		return root;
	}

	void dataFromJson(json_t* root) override {

		json_t *ck = json_object_get(root, "currentTheka");
		if(ck) {
			currentTheka = json_integer_value(ck);
		}

	}	

    void doStep() {

        // ADVANCE currentStep
        ++currentStep;
        if (currentStep >= thekalib.thekas[currentTheka].numBeats) {
            currentStep = 0;
        }

        // SET Lights
        if (currentStep == 0) {
            lights[SAM_LED].setBrightness(0.5); 
        } else {
            lights[SAM_LED].setBrightness(0.0);
        }
        if (thekalib.thekas[currentTheka].accents[currentStep] == 1) {
            lights[ACC_LED].setBrightness(0.5); 
        } else {
            lights[ACC_LED].setBrightness(0.0);
        }
        
 
        // PLAY next BOL
        std::string currentBolName = thekalib.thekas[currentTheka].bols[currentStep];
        int currentBolNum = bolNums.at(currentBolName);
        if (bols[currentBolNum].isReadyToPlay) {bols[currentBolNum].Play();}
        accent = thekalib.thekas[currentTheka].accents[currentStep];
        if (currentBolNum < NUM_BOL_BUTTONS)  {
             lightGates[currentBolNum].ReTrigger();
        }

    }


    void onSampleRateChange(const SampleRateChangeEvent& e) override {

        // To avoid clicks and pops, we are going to fade out the sound
        // before resampling, then fade back in. 

        sampleRate = e.sampleRate;

        float duration = 0.5;

        accentGate.Init(sampleRate);
        accentGate.SetDuration(0.125);

        silentMode = true;

        fadeOutGate.Init(sampleRate);
        fadeInGate.Init(sampleRate);
        fadeOutGate.SetDuration(duration);
        fadeInGate.SetDuration(duration);
        fadeOutGate.ReTrigger();
        silentGate.Init(sampleRate);
        silentGate.SetDuration(0.5);

        for (int i = 0; i < NUM_BOL_BUTTONS; ++ i) {
            lightGates[i].Init(sampleRate);
            lightGates[i].SetDuration(0.25);
        }
        
        for (int l = 0; l < NUM_BOLS; ++l) {
            bols[l].isPlaying = false;
        }

    }

    void reloadSamples() {
        // Reload samples at new sample rate
        for (int l = 0; l < NUM_BOLS; ++l) {
            bols[l].reLoad();
        }
        silentGate.ReTrigger();
    }

	void process(const ProcessArgs& args) override {

        // Handle button lights
        for (int i = 0; i < NUM_BOL_BUTTONS; ++ i) {
            lightGates[i].Process();
            lightGates[i].SetDuration(triggerGapSeconds/2);
            lights[i].setBrightness(lightGates[i].GetCurrentState());
        }

        // Fading in and fadingout gates are for live sample rate changes
        fadeInGate.Process();
        fadeOutGate.Process();
        silentGate.Process();
        fadingIn = fadeInGate.GetCurrentState();
        fadingOut = fadeOutGate.GetCurrentState();
        waiting = silentGate.GetCurrentState(); 
        if (!fadingOut && fadingOutPrev) {
            //done fading out
            reloadSamples();
        }
        if (!waiting && waitingPrev) {
            //done waiting
            fadeInGate.ReTrigger();
        }
        if (!fadingIn && fadingInPrev) {
            //done fading in
            silentMode = false;
        }
        fadingOutPrev = fadingOut;
        fadingInPrev = fadingIn;
        waitingPrev = waiting;

        // Set PLAYING light
        if (isPlaying) {
            lights[PLAYING_LED].setBrightness(0.5);
        } else {
            lights[PLAYING_LED].setBrightness(0.0);
        }

        // Check mode switch
        mode = params[MODESWITCH_PARAM].getValue();

        // Check for RESET trigger
        float currentResetTrig =  resetTrig.process(rescale(inputs[RESET_INPUT].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
        if (currentResetTrig && !prevClockTrig) {
            currentStep = -1;
        }
        prevResetTrig = currentResetTrig;

        // Check for CLOCK trigger
        float currentClockTrig =  clockTrig.process(rescale(inputs[CLOCK_INPUT].getVoltage(),0.1f, 2.0f, 0.f, 1.f));

        // Decide if we are still in play mode
        if (triggerGapSeconds == 0) {triggerGapSeconds = 0.5;}
        if ((APP->engine->getFrame() - prevFrame) > (triggerGapSeconds * sampleRate) && isPlaying) {
            //Been a while since there was a trigger, we aren't in play mode any more
            //turn off all lights
            for (int l = 0; l < NUM_BOL_BUTTONS; ++l) {
                lights[l].setBrightness(0.0);
            }      
            lights[ACC_LED].setBrightness(0.0);
            lights[SAM_LED].setBrightness(0.0);
            accent = 0; 
            isPlaying = false;
        }

        //Deal with CLOCK TRIG
        if (currentClockTrig && !prevClockTrig) {
            
            //Calculate time since last trigger to adjust gate times
			currentFrame = APP->engine->getFrame();
			trigToTrigTime = currentFrame - prevFrame;
			prevFrame = currentFrame;
			numRecentTriggers += 1;
			numTotalTriggers += 1;
			if (trigToTrigTime > (sampleRate * 5) && isPlaying) {
				//if there has been a really long gap, we reset (how long is too long?)
				numTotalTriggers = 0;
				numRecentTriggers = 0;
				triggerGapAccumulator = 0;
			}
			if (numTotalTriggers > 2) {
				triggerGapAccumulator += trigToTrigTime;
				triggerGapAverage = triggerGapAccumulator/numRecentTriggers;
				triggerGapSeconds = triggerGapAverage / sampleRate;
			} else {
				triggerGapSeconds = 0.125;
			}

			if (numRecentTriggers > numTriggersToAverage) {
				numRecentTriggers = 0;
				triggerGapAccumulator = 0;
			}

            isPlaying = true;
            doStep();
		}
        prevClockTrig = currentClockTrig;

        //Check for ACCENT trigger
        float currentAccTrig = accTrig.process(rescale(inputs[ACC_INPUT].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
        if (currentAccTrig && !prevAccTrig && !isPlaying) {
            accentGate.ReTrigger();
            accent = 1;
        } 
        accentGate.Process();
        if (!isPlaying) {
            accent = accentGate.GetCurrentState();
        }
        if (accent) {
            lights[ACC_LED].setBrightness(0.5);
        } else {
            lights[ACC_LED].setBrightness(0.0);
        }

        // Detect presses of UP and DOWN buttons
        if (upButtonTrig.process(params[BUTTONUP_PARAM].getValue())) {
            ++currentTheka;
            if (currentTheka >= NUM_THEKAS) {currentTheka = 0;}
            
        }
        if (downButtonTrig.process(params[BUTTONDOWN_PARAM].getValue())) {
            --currentTheka;
            if (currentTheka < 0 ) {currentTheka = NUM_THEKAS-1;}
        }

        //Deal with bol BUTTONS or bol TRIGGERS
        for (int n = 0; n < NUM_BOL_BUTTONS; ++n) {

            bols[n].mode = mode;

            bool Trigger = false;
            float currentTrigger =  inputTrig[n].process(rescale(inputs[n].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
            if (currentTrigger && !previousTriggers[n]) {
                Trigger = true;
            }
            previousTriggers[n] = currentTrigger;
            if (buttonTrig[n].process(params[n].getValue()) || Trigger) {
                if (n < NUM_BOL_BUTTONS) {lights[n].setBrightness(0.0);}
                if (bols[n].isReadyToPlay) {bols[n].Play();}
            } 
        }

        // Get sample audio
        float sigL, sigR;
        sigL = 0;
        sigR = 0;

        for (int n = 0; n < NUM_BOLS; ++n) {
            if (bols[n].isPlaying && bols[n].isReadyToPlay) {
                if (bols[n].current_sample.currentSample >= bols[n].current_sample.numSamples) {
                    bols[n].isPlaying = false;
                } else {
                    float curraccent;
                    if (accent) {
                        curraccent = accentfactor;
                    } else {
                        curraccent = 1;
                    }
                    audioSample thisSample = bols[n].current_sample.getSample();
                    sigL += (thisSample.left * curraccent);
                    sigR += (thisSample.right * curraccent);
                }
            } 
        }
        if (!silentMode) {
            outputs[AUDIOOUTL_OUTPUT].setVoltage(sigL*gain);
            outputs[AUDIOOUTR_OUTPUT].setVoltage(sigR*gain);
        } else {
            // We're fading in or out
            float fade;
            if (fadingIn) {
                fade = fadeInGate.GetElapsed()/fadeInGate.GetDuration();
            }
            if (fadingOut) {
                fade = fadeOutGate.GetElapsed()/fadeOutGate.GetDuration();
            }
            if (!fadingOut && !fadingIn) {
                fade = 0.98;
            }
            outputs[AUDIOOUTL_OUTPUT].setVoltage(sigL*gain*(1-fade));
            outputs[AUDIOOUTR_OUTPUT].setVoltage(sigR*gain*(1-fade));
        }
    }
};

struct TalaWidget : ModuleWidget {

    SeasideDigitalDisplay* talaDisplay;
    SeasideDigitalDisplay* beatDisplay;

    //Tala* module_;

	TalaWidget(Tala* module) {
        //module_ = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Tala.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(18.958, 31)), module, Tala::LIGHT1));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(31.59, 31)), module, Tala::LIGHT2));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(44.222, 31)), module, Tala::LIGHT3));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(56.854, 31)), module, Tala::LIGHT4));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(69.486, 31)), module, Tala::LIGHT5));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(82.118, 31)), module, Tala::LIGHT6));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(19.108, 49)), module, Tala::LIGHT7));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(31.74, 49)), module, Tala::LIGHT8));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(44.372, 49)), module, Tala::LIGHT9));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(57.0045, 49)), module, Tala::LIGHT10));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(69.636, 49)), module, Tala::LIGHT11));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(82.268, 49)), module, Tala::LIGHT12));

        addParam(createParamCentered<TablaPad>(mm2px(Vec(18.958, 31)), module, Tala::BUTTON1_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(31.59, 31)), module, Tala::BUTTON2_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(44.222, 31)), module, Tala::BUTTON3_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(56.854, 31)), module, Tala::BUTTON4_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(69.486, 31)), module, Tala::BUTTON5_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(82.118, 31)), module, Tala::BUTTON6_PARAM));

        addParam(createParamCentered<TablaPad>(mm2px(Vec(19.108, 49)), module, Tala::BUTTON7_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(31.74, 49)), module, Tala::BUTTON8_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(44.372, 49)), module, Tala::BUTTON9_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(57.0045, 49)), module, Tala::BUTTON10_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(69.636, 49)), module, Tala::BUTTON11_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(82.268, 49)), module, Tala::BUTTON12_PARAM));

        addParam(createParamCentered<UpButton>(mm2px(Vec(84.59, 74)), module, Tala::BUTTONUP_PARAM));
        addParam(createParamCentered<DownButton>(mm2px(Vec(84.59, 81)), module, Tala::BUTTONDOWN_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.958, 21)), module, Tala::INPUT1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.59, 21)), module, Tala::INPUT2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.222, 21)), module, Tala::INPUT3));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.854, 21)), module, Tala::INPUT4));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.486, 21)), module, Tala::INPUT5));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.118, 21)), module, Tala::INPUT6));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.108, 59)), module, Tala::INPUT7));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.74,  59)), module, Tala::INPUT8));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.372, 59)), module, Tala::INPUT9));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.0045, 59)), module, Tala::INPUT10));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.636, 59)), module, Tala::INPUT11));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.268, 59)), module, Tala::INPUT12));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13, 97)), module, Tala::RESET_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.7,  97)), module, Tala::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7,  77.2)), module, Tala::ACC_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(75.3,  97)), module, Tala::AUDIOOUTL_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(88.5,  97)), module, Tala::AUDIOOUTR_OUTPUT));

        addChild(createLightCentered<FlatMediumLight<GreenLight>>(mm2px(Vec(25.5,72.3)), module, Tala::SAM_LED));
        addChild(createLightCentered<FlatMediumLight<WhiteLight>>(mm2px(Vec(25.5,77.1)), module, Tala::ACC_LED));
        addChild(createLightCentered<FlatMediumLight<BlueLight>>(mm2px(Vec(25.5,82.2)), module, Tala::PLAYING_LED));

        addParam(createParamCentered<CKSSThree>(mm2px(Vec(9, 41)), module, Tala::MODESWITCH_PARAM));

        talaDisplay = new SeasideDigitalDisplay;    
        talaDisplay->box.pos = mm2px(Vec(51.5, 76));
        talaDisplay->setText("TEEN TAL");
        addChild(talaDisplay);

        beatDisplay = new SeasideDigitalDisplay(5);    
        beatDisplay->size = 8;
        beatDisplay->box.pos = mm2px(Vec(69, 72));
        beatDisplay->bgColor = nvgRGB(50,50,50);
	    beatDisplay->textColor = nvgRGB(255,255,255);
	    beatDisplay->blur1Color = nvgRGBA(200,200,200,200);
	    beatDisplay->blur2Color = nvgRGBA(50,50,50,200);
        beatDisplay->setText("0/0");
        addChild(beatDisplay);
    }
    
    void step() override {
        Tala *module = dynamic_cast<Tala*>(this->module); 
		if(module == NULL) return;
        
        if (module) {
            talaDisplay->setText(module->thekalib.thekas[module->currentTheka].name);
            std::string currStep;
            std::string totalSteps;

            totalSteps = std::to_string(module->thekalib.thekas[module->currentTheka].numBeats);
            if (module->thekalib.thekas[module->currentTheka].numBeats < 10) {
                totalSteps = "0" + totalSteps;
            }
            if (module->currentStep+1 > module->thekalib.thekas[module->currentTheka].numBeats) {
                currStep = "00";
            } else {
                currStep = std::to_string(module->currentStep+1);
                if (module->currentStep < 9) {
                    currStep = "0" + currStep;
                }
            }
            beatDisplay->setText(currStep + "/" + totalSteps);
        }      

        Widget::step();

    }

};



Model* modelTala = createModel<Tala, TalaWidget>("Tala");