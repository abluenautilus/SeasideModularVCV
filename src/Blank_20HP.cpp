#include "plugin.hpp"

struct Blank_20HP : Module {

    Blank_20HP() {

    }

};

struct Blank_20HPWidget : ModuleWidget {

    Blank_20HPWidget(Blank_20HP* module) {

        setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Blank_20HP.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    }

};

Model* modelBlank_20HP = createModel<Blank_20HP, Blank_20HPWidget>("Blank_20HP");