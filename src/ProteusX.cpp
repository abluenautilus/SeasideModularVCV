#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/formantfilter.hpp"
#include <random>

#define NUM_SLOTS 5
#define LIGHT_OFF_VALUE 0.01
#define LIGHT_ON_VALUE 1.0

struct proteusMessage {
    Note sequence[32];
    int restorder[32];
    int loadButtonPressed;
    float transposeValue;
    int restValue;
    std::string type;
};

struct ProteusX : Module {

    enum ParamId {
        BUTTON1_SAVE_PARAM,
        BUTTON1_LOAD_PARAM,
        BUTTON1_CLEAR_PARAM,
        BUTTON2_SAVE_PARAM,
        BUTTON2_LOAD_PARAM,
        BUTTON2_CLEAR_PARAM,
        BUTTON3_SAVE_PARAM,
        BUTTON3_LOAD_PARAM,
        BUTTON3_CLEAR_PARAM,
        BUTTON4_SAVE_PARAM,
        BUTTON4_LOAD_PARAM,
        BUTTON4_CLEAR_PARAM,
        BUTTON5_SAVE_PARAM,
        BUTTON5_LOAD_PARAM,
        BUTTON5_CLEAR_PARAM,
        POT1_PARAM,
		POT2_PARAM,
        POT3_PARAM,
        POT4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		CV2_INPUT,
        CV3_INPUT,
        CV4_INPUT,
        CV5_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUT1_OUTPUT,
		AUDIOOUT2_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
		LED1_LIGHT_RED,
		LED1_LIGHT_GREEN,
		LED1_LIGHT_BLUE,
        LED_BUTTON1_LIGHT_RED,
        LED_BUTTON1_LIGHT_BLUE,
        LED_BUTTON1_LIGHT_GREEN,
        LED_BUTTON2_LIGHT_RED,
        LED_BUTTON2_LIGHT_BLUE,
        LED_BUTTON2_LIGHT_GREEN,
        LED_BUTTON3_LIGHT_RED,
        LED_BUTTON3_LIGHT_BLUE,
        LED_BUTTON3_LIGHT_GREEN,
        LED_BUTTON4_LIGHT_RED,
        LED_BUTTON4_LIGHT_BLUE,
        LED_BUTTON4_LIGHT_GREEN,
        LED_BUTTON5_LIGHT_RED,
        LED_BUTTON5_LIGHT_BLUE,
        LED_BUTTON5_LIGHT_GREEN,
        LED_LOADBUTTON1_LIGHT_RED,
        LED_LOADBUTTON1_LIGHT_BLUE,
        LED_LOADBUTTON1_LIGHT_GREEN,
        LED_LOADBUTTON2_LIGHT_RED,
        LED_LOADBUTTON2_LIGHT_BLUE,
        LED_LOADBUTTON2_LIGHT_GREEN,
        LED_LOADBUTTON3_LIGHT_RED,
        LED_LOADBUTTON3LIGHT_BLUE,
        LED_LOADBUTTON3_LIGHT_GREEN,
        LED_LOADBUTTON4_LIGHT_RED,
        LED_LOADBUTTON4_LIGHT_BLUE,
        LED_LOADBUTTON4_LIGHT_GREEN,
        LED_LOADBUTTON5_LIGHT_RED,
        LED_LOADBUTTON5_LIGHT_BLUE,
        LED_LOADBUTTON5_LIGHT_GREEN,
		LIGHTS_LEN
	};

    //expander messages
    proteusMessage leftMessages[2][1] = {};
    bool button1Pressed;
	dsp::SchmittTrigger buttonTrigs[NUM_SLOTS];
    bool buttonStatesSavePrev[NUM_SLOTS];
    bool buttonStatesLoadPrev[NUM_SLOTS];
    bool buttonStatesClearPrev[NUM_SLOTS];

    //The sequences
	static const int maxSteps = 32;
    Note sequences[NUM_SLOTS][32];
    int restorders[NUM_SLOTS][32];

    bool slotFull[NUM_SLOTS];

    bool isConnected, wasConnected = false;

    int currentPattern = 0;

    //Prev trigger
	dsp::SchmittTrigger prevtrig;
	bool triggerStatePrev = false;
	bool prevTriggerStatePrev = false;
	float triggerValuePrev;

    //Next trigger
	dsp::SchmittTrigger nexttrig;
	bool triggerStateNext = false;
	bool prevTriggerStateNext = false;
	float triggerValueNext;

    //Rand trigger
	dsp::SchmittTrigger randtrig;
	bool triggerStateRand = false;
	bool prevTriggerStateRand = false;
	float triggerValueRand;

    //Random
    std::default_random_engine rng;
    float restValue, restValuePrev = 0.0f;
    float transposeValue, transposeValuePrev = 0.0f;

    ProteusX() {

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());

		configParam(POT1_PARAM, 0.0f, 11.0f, 0.0f, "Transpose"," semitones");
		paramQuantities[POT1_PARAM]->snapEnabled = true;
        configParam(POT2_PARAM, 0.0f, 32.0f, 0.0f, "Rest"," beats");
		paramQuantities[POT2_PARAM]->snapEnabled = true;
 
        leftExpander.producerMessage = leftMessages[0];
		leftExpander.consumerMessage = leftMessages[1];
        leftMessages[0][0].type = "producer";
        leftMessages[1][0].type = "consumer";

        //set lights to initial posiiton
        for (int i = 0; i < NUM_SLOTS; ++i) {
            slotFull[i] = false;
            buttonStatesSavePrev[i] = false;
            buttonStatesLoadPrev[i] = false;
            buttonStatesClearPrev[i] = false;
       
            int startNum = (i*3) + 3;
            if (leftExpander.module && (leftExpander.module->model == modelProteus)) {
                lights[startNum].setBrightness(0.0);
                lights[startNum+1].setBrightness(LIGHT_OFF_VALUE);
                lights[startNum+2].setBrightness(0.0);
            } else {
                lights[startNum].setBrightness(0.0);
                lights[startNum+1].setBrightness(0.0);
                lights[startNum+2].setBrightness(0.0);
            }
            
        }

    }


    json_t *dataToJson() override {
		json_t *root = json_object();

		json_object_set_new(root, "moduleVersion", json_integer(2));
		
		json_t *seq = json_array();
        for (int s = 0; s < NUM_SLOTS; s++) {
            for (int i = 0; i < 32; i++) {
                int y = 32 * s + i;
                if (slotFull[s]) {
                    json_array_insert_new(seq, y, json_integer(sequences[s][i].noteNumMIDI));
                } else {
                    json_array_insert_new(seq, y, json_integer(60));
                }
            }
        }

        json_t *ro = json_array();
        for (int s = 0; s < NUM_SLOTS; s++) {
            for (int i = 0; i < 32; i++) {
                int y = 32 * s + i;
                if (slotFull[s]) {
                    json_array_insert_new(ro, y, json_integer(restorders[s][i]));
                } else {
                    json_array_insert_new(ro, y, json_integer(0));
                }
            }
        }

        json_t *slots = json_array();
        for (int i = 0; i < NUM_SLOTS; ++i) {
            json_array_insert_new(slots, i, json_integer(slotFull[i]));
        }

		json_object_set_new(root, "sequences", seq);
        json_object_set_new(root, "restorders", ro);
        json_object_set_new(root, "slotsfull", slots);

		json_object_set_new(root, "transposeValue", json_integer(restValue));
		json_object_set_new(root, "restValue", json_integer(transposeValue));

		return root;
    }

    void dataFromJson(json_t* root) override {

		json_t *seq = json_object_get(root, "sequences");
		
        for (int s = 0; s < NUM_SLOTS; ++s) {
            for (int i = 0; i < 32; i++) {
                int y = 32 * s + i;
                if (seq) {
                    json_t *x = json_array_get(seq, y);
                    if (x) {
                        int MIDInum = json_integer_value(x);
                        Note nextNote = Note(MIDInum);
                        sequences[s][i] = nextNote;
                    }
                }
            }
        }

        json_t *ro = json_object_get(root, "restorders");
        for (int s = 0; s < NUM_SLOTS; ++s) {
            for (int i = 0; i < 32; i++) {
                int y = 32 * s + i;
                if (seq) {
                    json_t *x = json_array_get(ro, y);
                    if (x) {
                        restorders[s][i] = json_integer_value(x);
                    }
                }
            }
        }

        json_t *slots = json_object_get(root,"slotsfull");
        for (int i = 0; i < NUM_SLOTS; ++i) {
            json_t *x = json_array_get(slots, i);
            if (x) {
                int slot = json_integer_value(x);
                slotFull[i] = bool(slot);
            }
        }

        json_t *tv = json_object_get(root, "transposeValue");
		if(tv) {
			transposeValue = json_integer_value(tv);
		}

		json_t *rv = json_object_get(root, "restValue");
		if(rv) {
			restValue = json_integer_value(rv);
		}
    }

    void doLoad(int button) {
        
        if (slotFull[button]) {
        
            currentPattern = button;

            for (int i = 0; i < NUM_SLOTS; ++i) {
                int startNum = (i*3) + 18;
                if (i == currentPattern) {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_ON_VALUE);
                } else {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_OFF_VALUE);
                }
            }
            if (leftExpander.module && (leftExpander.module->model == modelProteus)) {

                //Outoing data to Proteus
                proteusMessage *messageToExpander = (proteusMessage*)(leftExpander.module->rightExpander.consumerMessage);
                messageToExpander[0].loadButtonPressed = true;
                for (int a = 0; a < 32; ++a) {
                    messageToExpander[0].sequence[a] = sequences[button][a];
                    messageToExpander[0].restorder[a] = restorders[button][a];
                }

            }
        }

    }
    void doSave (int button){


        if (!slotFull[button]) {

            slotFull[button] = true;

            currentPattern = button;

            for (int i = 0; i < NUM_SLOTS; ++i) {
                int startNum = (i*3) + 18;
                if (i == currentPattern) {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_ON_VALUE);
                } else {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_OFF_VALUE);
                }
            }

            if (leftExpander.module && (leftExpander.module->model == modelProteus)) {

                //Incoming data from Proteus
                proteusMessage *messagesFromExpander = (proteusMessage*)(leftExpander.producerMessage);
                for (int a = 0; a < 32; ++a) {
                    sequences[button][a] = messagesFromExpander[0].sequence[a];
                    restorders[button][a] = messagesFromExpander[0].restorder[a];
                }


            }
            //Change LED
            int startNum = (button*3) + 3;
            lights[startNum].setBrightness(0.0);
            lights[startNum+1].setBrightness(LIGHT_ON_VALUE);
            lights[startNum+2].setBrightness(0.0);
        }
   
    }

    void doClear (int button) {


        slotFull[button] = false;

        int startNum = (button*3) + 3;
        lights[startNum].setBrightness(0.0);
        lights[startNum+1].setBrightness(LIGHT_OFF_VALUE);
        lights[startNum+2].setBrightness(0.0);
    }
    
    void doNext() {

        currentPattern++;
        if (currentPattern >= NUM_SLOTS) {
            currentPattern  = 0;
        }
        int iter = 0;
        while (!slotFull[currentPattern] && iter < 4) {
            currentPattern++;
            iter++;
            if (currentPattern >= NUM_SLOTS) {
                currentPattern  = 0;
            }
        }

        doLoad(currentPattern);
    }

    void doPrev() {
        currentPattern--;
        if (currentPattern < 0) {
            currentPattern  = NUM_SLOTS;
        }
        int iter = 0;
        while (!slotFull[currentPattern] && iter < 4) {
            iter++;
            currentPattern--;
            if (currentPattern < 0) {
                currentPattern  = NUM_SLOTS;
            }

        }
        doLoad(currentPattern);

    }

    void doRand() {

         currentPattern = rand() % 5;
    
        int iter = 0;
        while (!slotFull[currentPattern] && iter < 4) {
            iter++;
            currentPattern = rand() % 5;
        }

        doLoad(currentPattern); 
    }

	void process(const ProcessArgs& args) override {

        //Process buttons
        for (int button = 0; button < NUM_SLOTS; ++button) {

            bool saveState = buttonTrigs[button].process(params[3*button].getValue());
            bool loadState = buttonTrigs[button].process(params[3*button+1].getValue());
            bool clearState = buttonTrigs[button].process(params[3*button+2].getValue());

            float transposeCV = inputs[CV4_INPUT].getVoltage() * 11;
            float restCV = inputs[CV5_INPUT].getVoltage() * 32;
            float transposeKnob = params[POT1_PARAM].getValue();
            float restKnob = params[POT2_PARAM].getValue();

            transposeValue = transposeCV + transposeKnob;
            if (transposeValue > 11) {transposeValue = 11;}
            if (transposeValue < 0) {transposeValue = 0;}   

            restValue = restCV + restKnob;
            if (restValue > 32) {restValue = 32;}
            if (restValue < 0) {restValue = 0;}       

            if (transposeValue != transposeValuePrev || restValue != restValuePrev) {
                if (leftExpander.module && (leftExpander.module->model == modelProteus)) {
                    //Outoing data to Proteus
                    proteusMessage *messageToExpander = (proteusMessage*)(leftExpander.module->rightExpander.consumerMessage);
                    messageToExpander[0].transposeValue = transposeValue;
                    messageToExpander[0].restValue = restValue;
                }                

            }  
            transposeValuePrev = transposeValue;
            restValuePrev = restValue;
            
            //SAVE buttons
            if (saveState && !buttonStatesSavePrev[button]) {
                buttonStatesSavePrev[button] = true;
                doSave(button);
            } 
            //LOAD buttons
            if (loadState && !buttonStatesLoadPrev[button]) {
                buttonStatesLoadPrev[button] = true;
                doLoad(button);
            } 
            //CLEAR buttons
            if (clearState && !buttonStatesClearPrev[button]){
                buttonStatesClearPrev[button] = true;
                doClear(button);
            }
        
            buttonStatesSavePrev[button] = saveState;
            buttonStatesLoadPrev[button] = loadState;
            buttonStatesClearPrev[button] = clearState;
        }

        //Record incoming triggers
		triggerValueNext = inputs[CV1_INPUT].getVoltage();
		nexttrig.process(rescale(triggerValueNext, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStateNext = triggerStateNext;
		triggerStateNext = nexttrig.isHigh();

        if (triggerStateNext & !prevTriggerStateNext) {
            doNext();
        }
        prevTriggerStateNext = triggerStateNext;

		triggerValuePrev = inputs[CV2_INPUT].getVoltage();
		prevtrig.process(rescale(triggerValuePrev, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStatePrev = triggerStatePrev;
		triggerStatePrev = prevtrig.isHigh();

        if (triggerStatePrev & !prevTriggerStatePrev) {
            doPrev();
        }
        prevTriggerStatePrev = triggerStatePrev;        

		triggerValueRand = inputs[CV3_INPUT].getVoltage();
		randtrig.process(rescale(triggerValueRand, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStateRand = triggerStateRand;
		triggerStateRand = randtrig.isHigh();

        if (triggerStateRand & !prevTriggerStateRand) {
            doRand();
        }
        prevTriggerStateRand = triggerStateRand;

        //Communication with Proteus
        bool proteusPresent = (leftExpander.module && leftExpander.module->model == modelProteus);
        if (proteusPresent & !wasConnected) {

            //Connect

            //turn on indictor light
            lights[LED1_LIGHT_RED].setBrightness(0); 
            lights[LED1_LIGHT_GREEN].setBrightness(0); 
            lights[LED1_LIGHT_BLUE].setBrightness(0.75); 

            for (int i = 0; i < NUM_SLOTS; ++i) {
                int startNum = (i*3) + 3;
                if (slotFull[i]) {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(LIGHT_ON_VALUE);
                    lights[startNum+2].setBrightness(0.0);
                } else {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(LIGHT_OFF_VALUE);
                    lights[startNum+2].setBrightness(0.0);
                }

                startNum = (i*3) + 18;
                if (i == currentPattern) {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_ON_VALUE);
                } else {
                    lights[startNum].setBrightness(0.0);
                    lights[startNum+1].setBrightness(0.0);
                    lights[startNum+2].setBrightness(LIGHT_OFF_VALUE);
                }
            }

            wasConnected = true;

        } else if (!proteusPresent & wasConnected) {

            //Disconnect
                
            lights[LED1_LIGHT_RED].setBrightness(1); 
            lights[LED1_LIGHT_GREEN].setBrightness(0); 
            lights[LED1_LIGHT_BLUE].setBrightness(0); 

            //set lights off
            for (int i = 0; i < NUM_SLOTS; ++i) {
                int startNum = (i*3) + 3;
                lights[startNum].setBrightness(0.0);
                lights[startNum+1].setBrightness(0.0);
                lights[startNum+2].setBrightness(0.0);

                startNum = (i*3) + 18;
                lights[startNum].setBrightness(0.0);
                lights[startNum+1].setBrightness(0.0);
                lights[startNum+2].setBrightness(0.0);
            }

            wasConnected = false;
        }

    }


};


struct ProteusXWidget : ModuleWidget {
	ProteusXWidget(ProteusX* module) {
        float xcenter = RACK_GRID_WIDTH*2-5;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ProteusX.svg")));

        float buttonXpositionA = 8.3;
        float buttonXpositionB = 16.3;
        float buttonXpositionC = 24.3;
        float buttonYposition = 27;
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionA, buttonYposition)), module, ProteusX::BUTTON1_SAVE_PARAM,ProteusX::LED_BUTTON1_LIGHT_RED));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionB, buttonYposition)), module, ProteusX::BUTTON1_LOAD_PARAM,ProteusX::LED_LOADBUTTON1_LIGHT_RED));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(buttonXpositionC, buttonYposition)), module, ProteusX::BUTTON1_CLEAR_PARAM));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionA, buttonYposition + 9)), module, ProteusX::BUTTON2_SAVE_PARAM,ProteusX::LED_BUTTON2_LIGHT_RED));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionB, buttonYposition + 9)), module, ProteusX::BUTTON2_LOAD_PARAM,ProteusX::LED_LOADBUTTON2_LIGHT_RED));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(buttonXpositionC, buttonYposition + 9)), module, ProteusX::BUTTON2_CLEAR_PARAM));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionA, buttonYposition + 18)), module, ProteusX::BUTTON3_SAVE_PARAM,ProteusX::LED_BUTTON3_LIGHT_RED));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionB, buttonYposition + 18)), module, ProteusX::BUTTON3_LOAD_PARAM,ProteusX::LED_LOADBUTTON3_LIGHT_RED));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(buttonXpositionC, buttonYposition + 18)), module, ProteusX::BUTTON3_CLEAR_PARAM));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionA, buttonYposition + 27)), module, ProteusX::BUTTON4_SAVE_PARAM,ProteusX::LED_BUTTON4_LIGHT_RED));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionB, buttonYposition + 27)), module, ProteusX::BUTTON4_LOAD_PARAM,ProteusX::LED_LOADBUTTON4_LIGHT_RED));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(buttonXpositionC, buttonYposition + 27)), module, ProteusX::BUTTON4_CLEAR_PARAM));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionA, buttonYposition + 35)), module, ProteusX::BUTTON5_SAVE_PARAM,ProteusX::LED_BUTTON5_LIGHT_RED));
        addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedGreenBlueLight>>>(mm2px(Vec(buttonXpositionB, buttonYposition + 35)), module, ProteusX::BUTTON5_LOAD_PARAM,ProteusX::LED_LOADBUTTON5_LIGHT_RED));
        addParam(createParamCentered<VCVButton>(mm2px(Vec(buttonXpositionC, buttonYposition + 35)), module, ProteusX::BUTTON5_CLEAR_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.5, 72)), module, ProteusX::CV1_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.3, 72)), module, ProteusX::CV2_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.1, 72)), module, ProteusX::CV3_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 88.5)), module, ProteusX::CV4_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 105)), module, ProteusX::CV5_INPUT));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 88.5)), module, ProteusX::POT1_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 105)), module, ProteusX::POT2_PARAM));

        addChild(createWidget<ScrewBlack>(Vec(xcenter, 0)));
        addChild(createWidget<ScrewBlack>(Vec(xcenter, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addChild(createLightCentered<SmallLight<RedGreenBlueLight>>(mm2px(Vec(2, 5)), module, ProteusX::LED1_LIGHT_RED));

    }
};

Model* modelProteusX = createModel<ProteusX, ProteusXWidget>("ProteusX");
