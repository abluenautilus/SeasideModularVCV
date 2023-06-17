#pragma once
#include "rack.hpp"
#include "componentlibrary.hpp"

static const NVGcolor SEASIDE_BLUE = nvgRGB(0x00, 0x22, 0x99);

//-----------------
//
// LIGHTS
//
//-----------------

template <typename TBase = app::ModuleLightWidget>
struct TSGrayModuleLightWidget : TBase {
	TSGrayModuleLightWidget() {
		this->bgColor = nvgRGBA(0x33, 0x33, 0x33, 0xff);
		this->borderColor = nvgRGBA(0, 0, 0, 53);
	}
};
using SSGrayModuleLightWidget = TSGrayModuleLightWidget<>;

template <typename TBase>
struct FlatMediumLight : TSvgLight<TBase> {
	FlatMediumLight() {
		this->setSvg(Svg::load((asset::plugin(pluginInstance,"res/Components/FlatMediumLight.svg"))));
	}
};

template <typename TBase = SSGrayModuleLightWidget>
struct TSBlueLight : TBase {
	TSBlueLight() {
		this->addBaseColor(SCHEME_BLUE);
	}
};
using SeasideBlueLight = TSBlueLight<>;



/** Based on the size of 5mm LEDs */
template <typename TBase = GrayModuleLightWidget>
struct TablaLight : TBase {
	TablaLight() {
		this->box.size = mm2px(math::Vec(8, 8));
	}
};

//-----------------
//
// SLIDERS
//
//-----------------

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

struct RubberSlider : app::SvgSlider {
	RubberSlider() {
		setBackgroundSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/RubberSlider.svg")));
		setHandleSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/RubberSliderHandleTop.svg")));
		setHandlePosCentered(
			math::Vec(8, 56),       //min value (bottom)
			math::Vec(8, 4)         //max value (top)
		);
	}
};

template <typename TBase>
struct RubberSliderLight : RectangleLight<TSvgLight<TBase>> {
	RubberSliderLight() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/RubberSliderLight.svg")));
	}
};

template <typename TLightBase = BlueLight>
struct RubberLightSlider : SeasideLightSlider<RubberSlider, RubberSliderLight<TLightBase>> {
	RubberLightSlider() {}
};

//-----------------
//
// JACKS
//
//-----------------

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

struct PJ301MPortBlack : app::SvgPort {
	PJ301MPortBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/PJ301M_black.svg")));
	}
};

// KNOBS

struct RoundBlackKnobEclipse : RoundKnob {
	RoundBlackKnobEclipse() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/PJ301M_black.svg")));
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Components/PJ301M_black.svg")));
	}
};

//-----------------
//
// SWITCHES
//
//-----------------

struct TablaPad : app::SvgSwitch {
	TablaPad() {
		momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/TablaCircle_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/TablaCircle_1.svg")));
	}
};

struct UpButton : app::SvgSwitch {
	UpButton() {
		momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/ButtonUp_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/ButtonUp_1.svg")));
	}
};

struct DownButton : app::SvgSwitch {
	DownButton() {
		momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/ButtonDown_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/ButtonDown_1.svg")));
	}
};

//-----------------
//
// BUTTONS
//
//-----------------v

struct SeasideButton : app::SvgSwitch {
	SeasideButton() {
		momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/SeasideButton_0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Components/SeasideButton_1.svg")));
	}
};
using LEDButton = SeasideButton;

struct SeasideLatch : SeasideButton {
	SeasideLatch() {
		momentary = false;
		latch = true;
	}
};

/** Looks best with MediumSimpleLight<WhiteLight> or a color of your choice.
*/
template <typename TLight>
struct SeasideLightButton : SeasideButton {
	app::ModuleLightWidget* light;

	SeasideLightButton() {
		light = new TLight;
		// Move center of light to center of box
		light->box.pos = box.size.div(2).minus(light->box.size.div(2));
		addChild(light);
	}

	app::ModuleLightWidget* getLight() {
		return light;
	}
};
template <typename TLight>
using LEDLightButton = SeasideLightButton<TLight>;

template <typename TLight>
struct SeasideLightLatch : SeasideLightButton<TLight> {
	SeasideLightLatch() {
		this->momentary = false;
		this->latch = true;
	}
};


//-----------------
//
// OTHER
//
//-----------------

struct SeasideDigitalDisplay : TransparentWidget {

    std::string text;
	std::string bgDigits;

    NVGalign horzAlignment = NVG_ALIGN_CENTER;
    NVGalign vertAlignment = NVG_ALIGN_TOP;
    int size = 16;
	unsigned long displayLength;
	float blur1 = 10.f;
    float blur2 = 8.f;
	
	//Colors
	NVGcolor bgColor = nvgRGB(10,50,80);
	NVGcolor textColor = nvgRGB(20,100,255);
	NVGcolor blur1Color = nvgRGBA(100,100,200,200);
	NVGcolor blur2Color = nvgRGBA(50,50,200,200);

    SeasideDigitalDisplay(unsigned long maxDisplayLength = 10) {
		displayLength = maxDisplayLength;
		bgDigits = std::string(maxDisplayLength, '~');

	}

    void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance,"res/fonts/Segment14.ttf"));
			if (font) {
				nvgFontSize(args.vg, size);
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 0.f);

				nvgFillColor(args.vg, bgColor);
				nvgTextAlign(args.vg, horzAlignment | vertAlignment);
				nvgText(args.vg, 0.f, 0.f, bgDigits.c_str(), NULL);

				nvgFillColor(args.vg, textColor);
				nvgTextAlign(args.vg, horzAlignment | vertAlignment);
				nvgText(args.vg, 0.f, 0.f, text.c_str(), NULL);

				nvgFillColor(args.vg, blur1Color);
				nvgTextAlign(args.vg, horzAlignment | vertAlignment);
				nvgFontBlur(args.vg, blur1);
				nvgText(args.vg, 0.f, 0.f, text.c_str(), NULL);

				nvgFillColor(args.vg, blur2Color);
				nvgTextAlign(args.vg, horzAlignment | vertAlignment);
				nvgFontBlur(args.vg, blur2);
				nvgText(args.vg, 0.f, 0.f, text.c_str(), NULL);
			}

		}
		Widget::drawLayer(args, layer);
	}

    void setText(const std::string& newText) {
		text = newText;
		if (text.size() > displayLength) {
			text.resize(displayLength);
		}
    	// std::replace(text.begin(), text.end(), ' ', '!');
	}

    void step() override {
		
		Widget::step();
	}

};

