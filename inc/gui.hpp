#pragma once
#include "plugin.hpp"
#include "Scales.hpp"

struct scaleKnob : ParamQuantity {

    std::string getDisplayValueString() override {

        return scaleNames.at(getValue());

    }


};