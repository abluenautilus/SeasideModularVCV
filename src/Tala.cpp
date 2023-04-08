#include "plugin.hpp"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"
#include "../inc/Bol.hpp"
#include "../inc/Thekas.hpp"

#define NUM_BOLS 12

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
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;

	dsp::SchmittTrigger buttonTrig[12];
    dsp::SchmittTrigger inputTrig[12];
    dsp::SchmittTrigger upButtonTrig;
    dsp::SchmittTrigger downButtonTrig;
    dsp::SchmittTrigger resetTrig;
    dsp::SchmittTrigger clockTrig;

    float previousTriggers[12];
    float prevResetTrig, prevClockTrig;

    Bol bols[12];
    int mode;
    float gain = 8.0;

    int currentTheka = 0; 
    int currentStep = 0;

    ThekaLibrary thekalib;

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

        for (int i = 0; i < 12; ++i) {
            bols[i] = Bol(BOLS[i]);
            bols[i].mode = mode;
        }
     
        lights[LIGHT1].setBrightness(1);
    }

    void doStep() {

        //ADVANCE currentStep
        ++currentStep;
        if (currentStep >= thekalib.thekas[currentTheka].numBeats) {
            currentStep = 0;
        }
        if (currentStep == 0) {
            lights[SAM_LED].setBrightness(1.0); 
        } else {
            lights[SAM_LED].setBrightness(0.0);
        }
 
        //PLAY next BOL
        std::string currentBolName = thekalib.thekas[currentTheka].bols[currentStep];
        int currentBolNum = bolNums.at(currentBolName);
        if (bols[currentBolNum].isReadyToPlay) {bols[currentBolNum].Play();}
        
        for (int l = 0; l < NUM_BOLS; ++l) {
            if (currentBolNum == l) {
                lights[l].setBrightness(1.0);
            } else {
                lights[l].setBrightness(0.0);
            }
        }

    }


    void onSampleRateChange(const SampleRateChangeEvent& e) override {

        sampleRate = e.sampleRate;

        //Turn off samples to avoid clicking 
        for (int l = 0; l < NUM_BOLS; ++l) {
            bols[l].isPlaying = false;
        }

        for (int l = 0; l < NUM_BOLS; ++l) {
            bols[l].reLoad();
        }

    }

	void process(const ProcessArgs& args) override {

        mode = params[MODESWITCH_PARAM].getValue();

        //Check for RESET
        float currentResetTrig =  resetTrig.process(rescale(inputs[RESET_INPUT].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
        if (currentResetTrig && !prevClockTrig) {
            //DO RESET
        }
        prevResetTrig = currentResetTrig;

        //Check for CLOCK
        float currentClockTrig =  clockTrig.process(rescale(inputs[CLOCK_INPUT].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
        if (currentClockTrig && !prevClockTrig) {
            doStep();
        }
        prevClockTrig = currentClockTrig;


        if (upButtonTrig.process(params[BUTTONUP_PARAM].getValue())) {
            ++currentTheka;
            if (currentTheka >= NUM_THEKAS) {currentTheka = 0;}
            
        }
        if (downButtonTrig.process(params[BUTTONDOWN_PARAM].getValue())) {
            --currentTheka;
            if (currentTheka < 0 ) {currentTheka = NUM_THEKAS-1;}
        }

        for (int n = 0; n < NUM_BOLS; ++n) {

            bols[n].mode = mode;

            bool Trigger = false;
            float currentTrigger =  inputTrig[n].process(rescale(inputs[n].getVoltage(),0.1f, 2.0f, 0.f, 1.f));
            if (currentTrigger && !previousTriggers[n]) {
                Trigger = true;
            }
            previousTriggers[n] = currentTrigger;
            if (buttonTrig[n].process(params[n].getValue()) || Trigger) {
                if (bols[n].isReadyToPlay) {bols[n].Play();}
            }
        }

        float sigL, sigR;
        sigL = 0;
        sigR = 0;
        for (int n = 0; n < NUM_BOLS; ++n) {
            if (bols[n].isPlaying && bols[n].isReadyToPlay) {
                if (bols[n].current_sample.currentSample >= bols[n].current_sample.numSamples) {
                    bols[n].isPlaying = false;
                } else {
                    audioSample thisSample = bols[n].current_sample.getSample();
                    sigL += thisSample.left;
                    sigR += thisSample.right;
                }
            }
        }
        outputs[AUDIOOUTL_OUTPUT].setVoltage(sigL*gain);
        outputs[AUDIOOUTR_OUTPUT].setVoltage(sigR*gain);
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
        
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(18.958, 28)), module, Tala::LIGHT1));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(31.59, 28)), module, Tala::LIGHT2));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(44.222, 28)), module, Tala::LIGHT3));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(56.854, 28)), module, Tala::LIGHT4));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(69.486, 28)), module, Tala::LIGHT5));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(82.118, 28)), module, Tala::LIGHT6));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(19.108, 46)), module, Tala::LIGHT7));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(31.74, 46)), module, Tala::LIGHT8));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(44.372, 46)), module, Tala::LIGHT9));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(57.0045, 46)), module, Tala::LIGHT10));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(69.636, 46)), module, Tala::LIGHT11));
        addChild(createLightCentered<TablaLight<BlueLight>>(mm2px(Vec(82.268, 46)), module, Tala::LIGHT12));

        addParam(createParamCentered<TablaPad>(mm2px(Vec(18.958, 28)), module, Tala::BUTTON1_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(31.59, 28)), module, Tala::BUTTON2_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(44.222, 28)), module, Tala::BUTTON3_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(56.854, 28)), module, Tala::BUTTON4_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(69.486, 28)), module, Tala::BUTTON5_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(82.118, 28)), module, Tala::BUTTON6_PARAM));

        addParam(createParamCentered<TablaPad>(mm2px(Vec(19.108, 46)), module, Tala::BUTTON7_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(31.74, 46)), module, Tala::BUTTON8_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(44.372, 46)), module, Tala::BUTTON9_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(57.0045, 46)), module, Tala::BUTTON10_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(69.636, 46)), module, Tala::BUTTON11_PARAM));
        addParam(createParamCentered<TablaPad>(mm2px(Vec(82.268, 46)), module, Tala::BUTTON12_PARAM));

        addParam(createParamCentered<UpButton>(mm2px(Vec(84.59, 72.68)), module, Tala::BUTTONUP_PARAM));
        addParam(createParamCentered<DownButton>(mm2px(Vec(84.59, 79.63)), module, Tala::BUTTONDOWN_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.958, 18)), module, Tala::INPUT1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.59, 18)), module, Tala::INPUT2));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.222, 18)), module, Tala::INPUT3));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.854, 18)), module, Tala::INPUT4));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.486, 18)), module, Tala::INPUT5));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.118, 18)), module, Tala::INPUT6));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.108, 56)), module, Tala::INPUT7));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.74,  56)), module, Tala::INPUT8));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.372, 56)), module, Tala::INPUT9));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.0045, 56)), module, Tala::INPUT10));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(69.636, 56)), module, Tala::INPUT11));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.268, 56)), module, Tala::INPUT12));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13, 97)), module, Tala::RESET_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.7,  97)), module, Tala::CLOCK_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(75.3,  97)), module, Tala::AUDIOOUTL_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(88.5,  97)), module, Tala::AUDIOOUTR_OUTPUT));

        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(25,72.1)), module, Tala::SAM_LED));

        addParam(createParamCentered<CKSSThree>(mm2px(Vec(9, 38)), module, Tala::MODESWITCH_PARAM));

        talaDisplay = new SeasideDigitalDisplay;    
        talaDisplay->box.pos = mm2px(Vec(51.5, 76));
        talaDisplay->setText("TEEN TAL");
        addChild(talaDisplay);

        beatDisplay = new SeasideDigitalDisplay(5);    
        beatDisplay->size = 8;
        beatDisplay->box.pos = mm2px(Vec(69, 72));
        // beatDisplay->bgColor = nvgRGB(10,80,50);
	    // beatDisplay->textColor = nvgRGB(20,255,100);
	    // beatDisplay->blur1Color = nvgRGBA(100,200,100,200);
	    // beatDisplay->blur2Color = nvgRGBA(50,200,50,200);
        beatDisplay->setText("0/0");
        addChild(beatDisplay);
    }
    
    void step() override {
        Tala *module = dynamic_cast<Tala*>(this->module); 
		if(module == NULL) return;
        
        if (module) {
            talaDisplay->setText(module->thekalib.thekas[module->currentTheka].name);
            std::string currStep;
            if (module->currentStep+1 > module->thekalib.thekas[module->currentTheka].numBeats) {
                currStep = "x";
            } else {
                currStep = std::to_string(module->currentStep+1);
            }
            beatDisplay->setText(currStep + "/" + std::to_string(module->thekalib.thekas[module->currentTheka].numBeats));
        }      

        Widget::step();

    }

};



Model* modelTala = createModel<Tala, TalaWidget>("Tala");