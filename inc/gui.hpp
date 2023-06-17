#pragma once
#include "plugin.hpp"
#include "Scales.hpp"

struct scaleKnob : ParamQuantity {

    std::string getDisplayValueString() override {

        return scaleNames.at(getValue());

    }


};

struct IndianNoteKnob : ParamQuantity {

    std::string getDisplayValueString() override {

        return saReGaMa.at(getValue());

    }


};

struct semitoneKnob : ParamQuantity {

    std::string getDisplayValueString() override {

        return semitoneNames.at(getValue());

    }


};