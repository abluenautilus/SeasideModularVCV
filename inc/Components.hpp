#pragma once
#include "rack.hpp"
#include "componentlibrary.hpp"

static const NVGcolor SEASIDE_BLUE = nvgRGB(0x00, 0x22, 0x99);

// LIGHTS

template <typename TBase = app::ModuleLightWidget>
struct TSGrayModuleLightWidget : TBase {
	TSGrayModuleLightWidget() {
		this->bgColor = nvgRGBA(0x33, 0x33, 0x33, 0xff);
		this->borderColor = nvgRGBA(0, 0, 0, 53);
	}
};
using SSGrayModuleLightWidget = TSGrayModuleLightWidget<>;

template <typename TBase = SSGrayModuleLightWidget>
struct TSBlueLight : TBase {
	TSBlueLight() {
		this->addBaseColor(SCHEME_BLUE);
	}
};
using SeasideBlueLight = TSBlueLight<>;

// Wide slider, used in Jawari
template <typename TBase, typename TLightBase = SeasideBlueLight>
struct SeasideLightSlider : TBase {
	app::ModuleLightWidget* light;

	SeasideLightSlider() {
		light = new TLightBase;
		this->addChild(light);
	}

	app::ModuleLightWidget* getLight() {
		return light;
	}

	void step() override {
		TBase::step();
		// Move center of light to center of handle
		light->box.pos = this->handle->box.pos
			.plus(this->handle->box.size.div(2))
			.minus(light->box.size.div(2));
	}
};

struct BigSlider : app::SvgSlider {
	BigSlider() {
		setBackgroundSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/BigSlider.svg")));
		setHandleSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/BigSliderHandle.svg")));
		setHandlePosCentered(
			math::Vec(8, 56),       //min value (bottom)
			math::Vec(8, 4)         //max value (top)
		);
	}
};

template <typename TBase>
struct BigSliderLight : RectangleLight<TSvgLight<TBase>> {
	BigSliderLight() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/BigSliderLight.svg")));
	}
};

template <typename TLightBase = BlueLight>
struct BigLightSlider : SeasideLightSlider<BigSlider, BigSliderLight<TLightBase>> {
	BigLightSlider() {}
};

// JACKS
struct PJ301MPortDark : app::SvgPort {
	PJ301MPortDark() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/PJ301M_dark.svg")));
	}
};

struct PJ301MPortBlue : app::SvgPort {
	PJ301MPortBlue() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/PJ301M_blue.svg")));
	}
};