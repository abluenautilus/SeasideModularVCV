#include "plugin.hpp"
#include "../inc/DaisySP/Source/daisysp.h"
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/Components.hpp"
#include "../inc/gui.hpp"

using namespace daisysp;

struct Eclipse : Module {

    enum ParamId {
		SEMI_PARAM,
        OCTAVE_PARAM,
        TUNING_PARAM,
        STRUMBUTTON_PARAM,
        RESETBUTTON_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        INPUT1,
        INPUT2,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIOOUT1_OUTPUT,
        AUDIOOUT2_OUTPUT,
		OUTPUTS_LEN
	};
    enum LightId {
        LIGHT1_LED,
		LIGHTS_LEN
	};

    // Sample rate
    float sampleRate;



    Eclipse() {

        

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(SEMI_PARAM, 0, 11, 0.0, "transpose"," semitones");
        paramQuantities[SEMI_PARAM]->snapEnabled = true;
        configParam(OCTAVE_PARAM, 0.0, 2, 0.0, "transpose"," octaves");
        paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
        configParam<IndianNoteKnob>(TUNING_PARAM, 1, 12, 8, "tuning (first string)");
        paramQuantities[TUNING_PARAM]->snapEnabled = true;

        configInput(INPUT1, "Input 1");
        configInput(INPUT2, "Input 2");

        configParam(STRUMBUTTON_PARAM, 0.f, 1.f, 0.f, "Strum");
        configParam(RESETBUTTON_PARAM, 0.f, 1.f, 0.f, "Reset");
        configOutput(AUDIOOUT1_OUTPUT, "Out");


        sampleRate = APP->engine->getSampleRate();

     
    }

	void process(const ProcessArgs& args) override {

      

    }
};


struct EclipseWidget : ModuleWidget {

    std::default_random_engine rng;

	EclipseWidget(Eclipse* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Eclipse.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        
        srand(time(NULL));
        int xmax = 50;
        int xmin = 5;
        int ymax = 120;
        int ymin = 25;
        int x = rand()%(xmax-xmin + 1) + xmin;
        int y = rand()%(ymax-ymin + 1) + ymin;

        addParam(createParamCentered<RoundBlackKnobEclipse>(mm2px(Vec(8.8, 88)), module, Eclipse::SEMI_PARAM));
        addParam(createParamCentered<RoundBlackKnobEclipse>(mm2px(Vec(25.4, 88)), module, Eclipse::OCTAVE_PARAM));
        addParam(createParamCentered<RoundBlackKnobEclipse>(mm2px(Vec(42, 88)), module, Eclipse::TUNING_PARAM));

        addInput(createInputCentered<PJ301MPortBlack>(mm2px(Vec(x, y)), module, Eclipse::INPUT1));
        addInput(createInputCentered<PJ301MPortBlack>(mm2px(Vec(25.4, 105)), module, Eclipse::INPUT2));

        addOutput(createOutputCentered<PJ301MPortBlack>(mm2px(Vec(32, 105)), module, Eclipse::AUDIOOUT1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortBlack>(mm2px(Vec(42, 105)), module, Eclipse::AUDIOOUT2_OUTPUT));

    }
};

Model* modelEclipse = createModel<Eclipse, EclipseWidget>("Eclipse");