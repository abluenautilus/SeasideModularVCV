#include "plugin.hpp"
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include "../inc/Note.hpp"
#include "../inc/Scales.hpp"
#include "../inc/poisson.hpp"
#include "../inc/gui.hpp"

#define GATE_HIGH_VOLTAGE 10.0f
#define LED_BRIGHTNESS_ON 1.0f

struct proteusMessage {
    Note sequence[32];
	int restorder[32];
    int loadButtonPressed;
	float transposeValue;
    int restValue;
	std::string type;
};

struct ProteusPlus : Module {
	enum ParamId {
		POT1_PARAM,
		POT2_PARAM,
		POT3_PARAM,
		POT4_PARAM,
		POT5_PARAM,
		POT6_PARAM,
		POT7_PARAM,
		BUTTON1_PARAM,
		SWITCH1_PARAM,
		SWITCH2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		CV2_INPUT,
		CV3_INPUT,
		CV4_INPUT,
		CV5_INPUT,
		CV6_INPUT,
		CV7_INPUT,
		CV8_INPUT,
		CLOCK_INPUT,
		RESET_INPUT,
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
		LED2_LIGHT_RED,
		LED2_LIGHT_GREEN,
		LED2_LIGHT_BLUE,
		LED3_LIGHT_RED,
		LED3_LIGHT_GREEN,
		LED3_LIGHT_BLUE,
		LED4_LIGHT_RED,
		LED4_LIGHT_GREEN,
		LED4_LIGHT_BLUE,
		LEDX_LIGHT_RED,
		LEDX_LIGHT_GREEN,
		LEDX_LIGHT_BLUE,
		LIGHTS_LEN
	};

	enum sequenceMutationOptions {
		DONT_MUTATE,
		CHANGE_NOTES,
		OCTAVE_UP,
		OCTAVE_DOWN,
		MUTATION_OPTIONS_LEN
	};
	enum newMelodyNoteOptions {
		NM_REPEAT,
		NM_UP,
		NM_DOWN,
		NM_NEW
	};

	//Clock trigger
	dsp::SchmittTrigger strig;
	bool triggerState = false;
	bool prevTriggerState = false;
	float triggerValue;

	//New melody button trigger
	dsp::SchmittTrigger patternResetTrig;

	//New melody CV trigger
	dsp::SchmittTrigger strigNewMelody;
	bool triggerStateNewMelody = false;
	bool prevTriggerStateNewMelody = false;
	float triggerValueNewMelody;

	//Reset trigger
	dsp::SchmittTrigger strigReset;
	bool triggerStateReset = false;
	bool prevTriggerStateReset = false;
	float triggerValueReset;

	//Lock toggle trigger
	dsp::SchmittTrigger strigLock;
	bool triggerStateLock = false;
	bool prevTriggerStateLock = false;
	float triggerValueLock;

	//boolean options
	bool mutate = true;
	bool accumulate = true;
	int mutate_octave_option = 0;
	int lock_option = 0;

	//The sequence
	static const int maxSteps = 32;
	Note sequence[maxSteps]; 

	//Melody creation
	int noteOptionWeights[4] = {5,5,5,10};
	std::string baseKey = "C";
	std::string scale = "Major";
	uint8_t scaleNum = 1;
	uint8_t baseOctave = 4;
	Note rootNote = Note(baseKey,baseOctave);
	std::vector<int> validTones = scaleTones.at(scale);
	std::vector<int> validToneWeights = scaleToneWeights.at(scale);
	
	//Gate generation
	float timePerTick = APP->engine->getSampleTime();
	float sampleRate = APP->engine->getSampleRate();
	dsp::PulseGenerator pulseGen;
	bool gateState = false;

	uint8_t repetitionCount = 0;
	uint8_t restingCount = 0;
	int octaveOffset = 0;
	uint8_t mutationOption = DONT_MUTATE;
	uint8_t maxOctaveOffsetUp = 2;
	uint8_t maxOctaveOffsetDown = 2;
	int sequenceLength = 16;
	int sequenceLengthPrev = 16;
	uint8_t sequenceGap = 0;
	double restProbability = 20; //out of 100
	double restProbabilityPrev = 20;
	float octaveChangeProbability = 20;
	float noteChangeProbability = 20;
	uint8_t currentNote = 0;
	float gateDuration = .05f;
	uint8_t activeLED = 0;
	float LEDbrightness = 0;
	double trigToTrigTime = 0;
	double prevTrigTime = 0;
	double prevFrame = 0;
	double currentFrame = 0;
	int numRecentTriggers = 0;
	double triggerGapAccumulator = 0;
	double triggerGapAverage = 0;
	double triggerGapSeconds = 0;
	static const int numTriggersToAverage = 6;
	float gateLengthKnobPosition;
	int numTotalTriggers = 0;
	bool newMelodyPressed = false;
	float restValue, transposeValue = 0;
	bool restMode = false;
	int restStep = 0;

	float octaveCV;
	float scaleCV;
	float gateLengthCV;
	float noteChangeCV;
	float lambdaCV;
	float densityCV;
	float modulateCV;
	float lengthCV;
	std::default_random_engine rng;

	uint8_t poisson_lambda = 12;

	Note prevNote;
		
	int noteKind;
	int noteOn;

	int numRestNotes = 0;
	int restorder[maxSteps];

	//Expander
	proteusMessage rightMessages[2][1] = {};

	ProteusPlus() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(POT1_PARAM, 1.0f, 32.0f, 16.0f, "Sequence length"," beats");
		paramQuantities[POT1_PARAM]->snapEnabled = true;
		configParam<scaleKnob>(POT2_PARAM, 1.f, 8.f, 1.f, "Scale");
		paramQuantities[POT2_PARAM]->snapEnabled = true;
		configParam(POT3_PARAM, 0.f, 10.f, 5.f, "Note Option REPEAT");
		paramQuantities[POT3_PARAM]->snapEnabled = true;
		configParam(POT4_PARAM, 0.f, 10.f, 5.f, "Note option UP");
		configParam(POT5_PARAM, 0.5f, 100.0f, 80.0f, "Density","%");
		paramQuantities[POT5_PARAM]->snapEnabled = true;
		configParam(POT6_PARAM, 0.0f, 10.f, 5.f, "Note option DOWN");
		paramQuantities[POT6_PARAM]->snapEnabled = true;
		configParam(POT7_PARAM, 0.f, 10.f, 10.f, "Note uption NEW");
		paramQuantities[POT7_PARAM]->snapEnabled = true;
		configParam(BUTTON1_PARAM, 0.f, 1.f, 0.f, "New sequence");
		configParam(SWITCH1_PARAM, 0.f, 2.f, 0.f, "Mode");
		configParam(SWITCH2_PARAM, 0.f, 2.f, 1.f, "Octave Range");
		configInput(CV1_INPUT, "Seq Length CV");
		configInput(CV2_INPUT, "Lock CV");
		configInput(CV3_INPUT, "Lambda CV");
		configInput(CV4_INPUT, "Gate Length CV");
		configInput(CV5_INPUT, "Density CV");
		configInput(CV6_INPUT, "Octave CV");
		configInput(CV7_INPUT, "Mutate CV");
		configInput(CV8_INPUT, "New Sequence Gate");
		configInput(CLOCK_INPUT, "Clock In");
		configInput(RESET_INPUT, "Reset");

		configOutput(AUDIOOUT1_OUTPUT, "CV 1V/OCT");
		configOutput(AUDIOOUT2_OUTPUT, "Gate");

		rightExpander.producerMessage = rightMessages[0];
		rightExpander.consumerMessage = rightMessages[1];

		densityCV = clamp(inputs[CV5_INPUT].getVoltage(),-10.0,10.0);
		lengthCV = clamp(inputs[CV1_INPUT].getVoltage(),-10.0,10.0);
		int lengthAdjustment = (maxSteps/10) * lengthCV;
		sequenceLength = clamp(int(params[POT1_PARAM].getValue()) + lengthAdjustment,1,maxSteps);
		restProbability = clamp(100 - params[POT5_PARAM].getValue() - (10 * densityCV),0.0,100.0); 

		rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
		newMelody();
	}

	json_t *dataToJson() override {

		json_t *root = json_object();

		json_object_set_new(root, "moduleVersion", json_integer(2));
		
		json_t *seq = json_array();
		for (int i = 0; i < 32; i++) {
			json_array_insert_new(seq, i, json_integer(sequence[i].noteNumMIDI));
		}

		json_t *ro = json_array();
		for (int i = 0; i < maxSteps; i++) {
			json_array_insert_new(ro, i, json_integer(restorder[i]));
		}

		json_object_set_new(root, "sequence", seq);
		json_object_set_new(root, "restorder", ro);
		json_object_set_new(root, "octaveOffset", json_integer(octaveOffset));
		json_object_set_new(root, "repetitionCount", json_integer(repetitionCount));
		json_object_set_new(root,"mutate_octave_option",json_integer(mutate_octave_option));
		json_object_set_new(root,"lock_option",json_integer(lock_option));
		json_object_set_new(root,"sequenceLength",json_integer(sequenceLength));

		return root;
	}

    int weightedRandom(int weights[],int num_choices) {
        int sum_of_weight = 0;

        for(int i=0; i<num_choices; i++) {
            sum_of_weight += weights[i];
        }
        int rnd = std::rand() % sum_of_weight;

        for(int i=0; i<num_choices; i++) {
            if(rnd < weights[i]) {
                return i;
            }
            rnd -= weights[i];
        }
        return -1;
    }

	void dataFromJson(json_t* root) override {


		json_t *seq = json_object_get(root, "sequence");
		
		for (int i = 0; i < 32; i++) {
			if (seq) {
				json_t *x = json_array_get(seq, i);
				if (x) {
					int MIDInum = json_integer_value(x);
					Note nextNote = Note(MIDInum);
					sequence[i] = nextNote;
				}
			}
		}

		json_t *sl = json_object_get(root, "sequenceLength");
		if(sl) {
			sequenceLength = json_integer_value(sl);
		}
		
		if (sequenceLength) {
			json_t *ro = json_object_get(root,"restorder");
			for (int i = 0; i < maxSteps; i++) {
				if (ro) {
					json_t *x = json_array_get(ro, i);
					if (x) {
						restorder[i] = json_integer_value(x);
					} 
				}
			}
		}

		json_t *oo = json_object_get(root, "octaveOffset");
		if(oo) {
			octaveOffset = json_integer_value(oo);
		}

		json_t *rc = json_object_get(root, "repetitionCount");
		if(rc) {
			repetitionCount = json_integer_value(rc);
		}

		json_t *mo = json_object_get(root, "mutate_octave_option");
		if(mo) {
			mutate_octave_option = json_integer_value(mo);
		}

		json_t *lo = json_object_get(root, "lock_option");
		if(lo) {
			lock_option = json_integer_value(lo);
		}


	}	

	void process(const ProcessArgs& args) override {

		//Record incoming triggers
		triggerValue = inputs[CLOCK_INPUT].getVoltage();
		strig.process(rescale(triggerValue, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerState = triggerState;
		triggerState = strig.isHigh();

		triggerValueReset = inputs[RESET_INPUT].getVoltage();
		strigReset.process(rescale(triggerValueReset, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStateReset = triggerStateReset;
		triggerStateReset = strigReset.isHigh();

		triggerValueLock = inputs[CV2_INPUT].getVoltage();
		strigLock.process(rescale(triggerValueLock, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStateLock = triggerStateLock;
		triggerStateLock = strigLock.isHigh();

		triggerValueNewMelody = inputs[CV8_INPUT].getVoltage();
		strigNewMelody.process(rescale(triggerValueNewMelody, 0.1f, 2.0f, 0.f, 1.f));
		prevTriggerStateNewMelody = triggerStateNewMelody;
		triggerStateNewMelody = strigNewMelody.isHigh();

		//Process regen button
		if (patternResetTrig.process(params[BUTTON1_PARAM].getValue())) {
			newMelody();
			newMelodyPressed = true;
		} else {
			newMelodyPressed = false;
		}

		//scale selection
		scaleNum = params[POT2_PARAM].getValue();

		//Process incoming CV
		gateLengthCV = clamp(inputs[CV4_INPUT].getVoltage(),-10.0f,10.0f);
		noteChangeCV = clamp(inputs[CV7_INPUT].getVoltage(),-10.0,10.0);
		lambdaCV = clamp(inputs[CV3_INPUT].getVoltage(),-10.0,10.0);
		densityCV = clamp(inputs[CV5_INPUT].getVoltage(),-10.0,10.0);
		octaveCV = clamp(inputs[CV6_INPUT].getVoltage(),-10.0,10.0);
		lengthCV = clamp(inputs[CV1_INPUT].getVoltage(),-10.0,10.0);
		int lengthAdjustment = (maxSteps/10) * lengthCV;

		//Process knob value changes
		sequenceLength = clamp(int(params[POT1_PARAM].getValue()) + lengthAdjustment,1,maxSteps);
		restProbability = clamp(100 - params[POT5_PARAM].getValue() - (10 * densityCV),0.0,100.0); 
		//poisson_lambda = clamp(int(params[POT3_PARAM].getValue()) + (5 * int(lambdaCV)),1,50);
		noteOptionWeights[0] = int(params[POT3_PARAM].getValue());
		noteOptionWeights[1] = int(params[POT4_PARAM].getValue());
		noteOptionWeights[2] = int(params[POT6_PARAM].getValue());
		noteOptionWeights[3] = int(params[POT7_PARAM].getValue());
		// octaveChangeProbability = clamp(params[POT6_PARAM].getValue() + (10 * octaveCV),0.0,100.0);
		// noteChangeProbability = clamp(params[POT7_PARAM].getValue() + (10 * noteChangeCV),0.0,100.0);
		// gateLengthKnobPosition = clamp(params[POT4_PARAM].getValue() + (gateLengthCV/10),0.1,1.0);

		//Check if density changed for live updating
		if (restProbability != restProbabilityPrev) {
			updateRests();
		}
		restProbabilityPrev = restProbability;
		
		//If sequence length changed we may need to adjust rests
		if (sequenceLength != sequenceLengthPrev) {
			if (sequenceLength > sequenceLengthPrev) {
				
				int numHidden = sequenceLength - sequenceLengthPrev;
				int hiddenRests[numHidden];
				int x = 0;
				for (int y=sequenceLengthPrev-1; y<sequenceLength; ++y) {
					hiddenRests[x] = y;
					++x;
				}
				
				std::shuffle(&hiddenRests[0],&hiddenRests[numHidden-1],rng);
				int counter = 0;
				for (int w = sequenceLengthPrev; w < sequenceLength; ++w) {
					restorder[w] = hiddenRests[counter];
					++counter;
				}
				updateRests();
			} else {
				updateRests();
			}
		} 

		sequenceLengthPrev = sequenceLength;

		//Process outgoing gates
		gateState = pulseGen.process(timePerTick);
		if(gateState) {
			outputs[AUDIOOUT2_OUTPUT].setVoltage(GATE_HIGH_VOLTAGE);
		} else {
			outputs[AUDIOOUT2_OUTPUT].setVoltage(0);
		}

		//Process switches
		if (params[SWITCH1_PARAM].getValue() == 0) {
			//Normal mode, mutation and regen both on
			mutate = true;
			accumulate = true;

		} else if (params[SWITCH1_PARAM].getValue() == 1) {
			//allow to regen but no mutations
			mutate = true;
			accumulate = false;

		} else if (params[SWITCH1_PARAM].getValue() == 2) {
			//no changes at all
			mutate = false;
			accumulate = false;
		}

		if (params[SWITCH2_PARAM].getValue()==0) {
			maxOctaveOffsetDown = 0;
			maxOctaveOffsetUp = 0;
		} else if (params[SWITCH2_PARAM].getValue()==1) {
			maxOctaveOffsetDown = 1;
			maxOctaveOffsetUp = 1;
		} else if (params[SWITCH2_PARAM].getValue()==2) {
			maxOctaveOffsetDown = 2;
			maxOctaveOffsetUp = 2;
		}

		//process lock trigger
		if (!prevTriggerStateLock && triggerStateLock) {
			if (lock_option==0) {
				if (accumulate) {
					accumulate = false;
					mutate = false;
					params[SWITCH1_PARAM].setValue(2);
				} else {
					mutate = true;
					accumulate = true;
					params[SWITCH1_PARAM].setValue(0);
				}
			} else if (lock_option==1) {
				if (params[SWITCH1_PARAM].getValue()>=1) {
					accumulate = true;
					mutate = true;
					params[SWITCH1_PARAM].setValue(0);
				} else {
					mutate = true;
					accumulate = false;
					params[SWITCH1_PARAM].setValue(1);
				}
			} else if (lock_option==2) {
				int p = params[SWITCH1_PARAM].getValue();
				if (p==0) {
					params[SWITCH1_PARAM].setValue(1);
					accumulate = false;
					mutate = true;
				} else if (p==1) {
					params[SWITCH1_PARAM].setValue(2);
					accumulate = false;
					mutate = false;
				} else if (p==2) {
					params[SWITCH1_PARAM].setValue(0);
					accumulate = true;
					mutate = true;
				}
			} else if (lock_option == 3) {
					accumulate = false;
					mutate = false;
					params[SWITCH1_PARAM].setValue(2);
			}
		}
		if (prevTriggerStateLock && !triggerStateLock) {
			if (lock_option==3){
				accumulate = true;
				mutate = true;
				params[SWITCH1_PARAM].setValue(0);
			}
		}
		


		//process new melody incoming trigger
		if (!prevTriggerStateNewMelody && triggerStateNewMelody) {
			newMelody();
		}

		//process reset trigger
		if (!prevTriggerStateReset && triggerStateReset) {
			currentNote = 0;
			activeLED=0;
		}

		
		//Check for incoming trigger and do the step when it comes in
		if (!prevTriggerState && triggerState) {
			
			//Calculate time since last trigger to adjust gate times
			currentFrame = APP->engine->getFrame();
			trigToTrigTime = currentFrame - prevFrame;
			prevFrame = currentFrame;
			numRecentTriggers += 1;
			numTotalTriggers += 1;
			if (trigToTrigTime > (sampleRate * 5)) {
				//if there has been a really long gap, we reset (how long is too long?)
				numTotalTriggers = 0;
				numRecentTriggers = 0;
				triggerGapAccumulator = 0;
			}
			if (numTotalTriggers > 2) {
				triggerGapAccumulator += trigToTrigTime;
				triggerGapAverage = triggerGapAccumulator/numRecentTriggers;
				triggerGapSeconds = triggerGapAverage / sampleRate;
				gateDuration = gateLengthKnobPosition * triggerGapSeconds;
				if (gateDuration < 0.01f) { gateDuration = 0.01f;}
			} else {
				gateDuration = 0.125f;
			}

			if (numRecentTriggers > numTriggersToAverage) {
				numRecentTriggers = 0;
				triggerGapAccumulator = 0;
			}

			doStep();
		}

		//Expander
		if (rightExpander.module && (rightExpander.module->model == modelProteusX )) {

				//indicator lights up to say we found expander
				lights[LEDX_LIGHT_RED].setBrightness(0); 
				lights[LEDX_LIGHT_GREEN].setBrightness(0); 
				lights[LEDX_LIGHT_BLUE].setBrightness(1); 

				// //Incoming data from expander
				proteusMessage *messagesFromExpander = (proteusMessage*)rightExpander.consumerMessage;
				bool loadButtonPressed = messagesFromExpander[0].loadButtonPressed;
				if (loadButtonPressed) {
					INFO("PROTEUS RECEIVED MESSAGE THAT BUTTON WAS PRESSED");
					for (int a =0; a < 32; ++a) {
						sequence[a] = messagesFromExpander[0].sequence[a];
					}
					rightMessages[1][0].loadButtonPressed = false;

				}
				transposeValue = messagesFromExpander[0].transposeValue;
				restValue = messagesFromExpander[0].restValue;

		} else {
			lights[LEDX_LIGHT_RED].setBrightness(0); //R
			lights[LEDX_LIGHT_GREEN].setBrightness(0); //G
			lights[LEDX_LIGHT_BLUE].setBrightness(0); //B
		}

	}

	void updateRests() {

		numRestNotes = ceil(sequenceLength*(restProbability/100));
		if (numRestNotes == sequenceLength) {--numRestNotes;} //always leave at least one note

		for (int x = 0; x < sequenceLength; ++x) {
			if (x < numRestNotes) {
				sequence[restorder[x]].muted = true;
			} else {
				sequence[restorder[x]].muted = false;
			}
		}
	}

	void transposeNote(Note* thenote, float transposeAmount) {
		int semitone = thenote->toneNum;
		semitone = semitone + transposeAmount;
		int octave = thenote->octave;
		if (semitone > 12) {
			semitone = semitone -12;
			octave = octave + 1;
		}
		if (semitone < 0) {
			semitone = semitone + 12;
			octave = octave -1;
		}	
		thenote->octave = octave;
		thenote->toneNum = semitone;
		thenote->setNoteNumMIDI();
		thenote->setVoltage();
	}

	void doStep() {


		double p,q,s,xlam,bound;
		int which,status;

		bool melodyChanged = false;

		//Process next step
			
		//turn all lights off
		for (int i = 1; i <= LIGHTS_LEN; ++i) {
			lights[i-1].setBrightness(0);
		}
		
		//but turn the next one on
		LEDbrightness = float(repetitionCount)/float(poisson_lambda);
		LEDbrightness = LEDbrightness/0.5;
		if (LEDbrightness < .05) {LEDbrightness = 0.05f;};

		if (currentNote == 0){
			lights[activeLED].setBrightness(0); //R
			lights[activeLED+1].setBrightness(1); //G
			lights[activeLED+2].setBrightness(0); //B
		} else {
			if (accumulate && mutate) {
				lights[activeLED].setBrightness(0); //R
				lights[activeLED+1].setBrightness(0); //G
				lights[activeLED+2].setBrightness(LEDbrightness); //B
			} else if (mutate && !accumulate) {
				lights[activeLED].setBrightness(0.33f); //R
				lights[activeLED+1].setBrightness(0); //G
				lights[activeLED+2].setBrightness(LEDbrightness); //B
			} else {
				lights[activeLED].setBrightness(0.16f); //R
				lights[activeLED+1].setBrightness(0.17f); //G
				lights[activeLED+2].setBrightness(LEDbrightness); //B
			}
		}
		
		activeLED+=3;
		if (activeLED >= LIGHTS_LEN) {activeLED=0;}

		Note noteToPlay = sequence[currentNote];
		transposeNote(&noteToPlay,transposeValue);

		if (restMode) {
			
			++restStep;
			INFO("Resting %d",restStep);

			if (restStep >= restValue) {
				restMode = false;
			}

		} else {

			//Play the current step's note
			if (!noteToPlay.muted) {
				//set the voltage
				outputs[AUDIOOUT1_OUTPUT].setVoltage(noteToPlay.voltage);

				//send a gate/trigger
				pulseGen.trigger(gateDuration);
				outputs[AUDIOOUT2_OUTPUT].setVoltage(GATE_HIGH_VOLTAGE);

			}
			
			++currentNote;

		}

		//Deal with end of loop scenarios
		if (currentNote >= sequenceLength) {

			//We are looping, so we need to check for various changes
			//accumulate means we are accruing repetitions
			if (accumulate) {
				repetitionCount++;

				//calculate poissoin cdf
				s = double(repetitionCount);
				xlam = poisson_lambda;
				which = 1;
				cdfpoi(&which, &p, &q, &s, &xlam, &status, &bound);

				//use cdf probability to decide if we get a new melody
				int choice = std::rand() % 100;
				if (choice < p*100) {
					newMelody();
					melodyChanged = true;
				} else {
					melodyChanged = false;
				}
			}
			if (!melodyChanged && mutate) {

				int octaveChoice = std::rand() % 100;
				if (octaveChoice < octaveChangeProbability) {
					int coinFlip = std::rand() %100;
					if (coinFlip < 50) {
						if (octaveOffset <=  -1*maxOctaveOffsetDown) {
							//We're already at the min octave so we bounce up
							changeOctave(1);
						} else {
							changeOctave(-1);
						}
					} else {
						if (octaveOffset >= maxOctaveOffsetUp) {
							//We're already at the max octave so we bounce down
							changeOctave(-1);
						} else {
							changeOctave(1);
						}
					}
					
				} else {
				}

				int noteChoice = std::rand() % 100;
				if (noteChoice < noteChangeProbability) {
					changeNotes(1);
				}
		
			}

			currentNote = 0;
			activeLED = 0;

			if (restValue > 0) {
				restStep = 0;
				restMode = true;
				INFO("ENTERING REST MODE resting for %.2f",restValue);
			} else {
				INFO("Restvalue is %.2f not resting",restValue);
			}
		}

		//Expander
		if (rightExpander.module && (rightExpander.module->model == modelProteusX )) {

				//Outgoing data to send to expander
				proteusMessage *messagesToExpander = (proteusMessage*)(rightExpander.module->leftExpander.producerMessage);
				messagesToExpander[0].loadButtonPressed = (bool)newMelodyPressed;
				for (int a = 0; a < 32; ++a) {
					messagesToExpander[0].sequence[a] = sequence[a];
					messagesToExpander[0].restorder[a] = restorder[a];
				}
					

		} 

	}

	void changeOctave(int octaveChange) {
		
		//transpose by octave
		
		if (octaveOffset+octaveChange > maxOctaveOffsetUp) {
	
		} else if (octaveOffset+octaveChange <  -1*maxOctaveOffsetDown) {
	
		} else {

			for (int t=0; t < maxSteps; ++t) {
				sequence[t].octave += octaveChange;
				sequence[t].setVoltage();
				sequence[t].setNoteNumMIDI();

			}
			octaveOffset += octaveChange;

		}
	
	}

	void changeNotes(int amount) {
		
		//substitute notes in the melody with new notes
		int noteToChange = std::rand() % sequenceLength;
		Note newNote = getRandomNoteMutate();
		//new notes should follow Density
		int noteOnChoice = std::rand() % 100;
		if (noteOnChoice < restProbability) {
			newNote.muted = true;
		} else {
			newNote.muted = false;
		}

		sequence[noteToChange] = newNote;

	}

	void newMelody() {

		scale = scaleNames.at(scaleNum);
		validTones = scaleTones.at(scale);
		validToneWeights = scaleToneWeights.at(scale);

		//shuffle rest order
		for (int i = 0; i<sequenceLength; ++i) {
			restorder[i] = i;
		}
		std::shuffle(std::begin(restorder),&restorder[sequenceLength-1],rng);
		numRestNotes = ceil(sequenceLength*(restProbability/100));
		if (numRestNotes == sequenceLength) {--numRestNotes;} //always leave at least one note

		//Generate a new melody from scratch

		//Flash lights
		for (int i = 0; i < LIGHTS_LEN; i+=3) {
			lights[i].setBrightness(1); //R
			lights[i+1].setBrightness(1); //G
			lights[i+2].setBrightness(1); //B
		}

		octaveOffset = 0;
		repetitionCount = 0;

		for (int x = 0; x < maxSteps; x++ ){
			
			//Decide what kind of note
			//First note is always a new note
			
			if (x>0){
				noteKind = weightedRandom(noteOptionWeights,4);
			} else {
				noteKind = NM_NEW;
			}

			if (noteKind == NM_REPEAT) {

				sequence[x] = prevNote;

			} else if (noteKind == NM_DOWN) {

				//find tone of previous note in the scale, find index of toneNum in validTones
				std::vector<int>::iterator it = std::find(validTones.begin(),validTones.end(),prevNote.toneNum);
				int toneIndex = std::distance(validTones.begin(), it);
				toneIndex--;
				int newOctave = prevNote.octave;
				if (toneIndex < 0) {
					toneIndex = validTones.size()-1;
					newOctave--;
				}
				int newTone = validTones[toneIndex];
			
				std::string newNoteName = prevNote.getNoteNameFromNum(newTone);

				Note aNewNote = Note(newNoteName,newOctave);
				sequence[x] = aNewNote;

			} else if (noteKind == NM_UP) {

				//find tone of previous note in the scale, find index of toneNum in validTones
				std::vector<int>::iterator it = std::find(validTones.begin(),validTones.end(),prevNote.toneNum);
				int toneIndex = std::distance(validTones.begin(), it);
				toneIndex++;
				int newOctave = prevNote.octave;
				if (toneIndex >= int(validTones.size())) {
					toneIndex = 0;
					newOctave++;
				}
				int newTone = validTones[toneIndex];
			
				std::string newNoteName = prevNote.getNoteNameFromNum(newTone);

				Note aNewNote = Note(newNoteName,newOctave);
				sequence[x] = aNewNote;


			} else if (noteKind == NM_NEW) {

				Note newNote = getRandomNote();
				sequence[x] = newNote;
				prevNote = newNote;
			}

			prevNote = sequence[x];		

		}
		for (int x = 0; x < sequenceLength; ++x) {
			if (x < numRestNotes) {
				sequence[restorder[x]].muted = true;
			} else {
				sequence[restorder[x]].muted = false;
			}
		}
	
	}

	Note getRandomNote() {

		//Get a random note from the scale

		int ourSemitone;
		int newNoteMIDI;
		int ourChoice;

		int num_choices = validToneWeights.size();
		ourChoice = weightedRandom(&validToneWeights[0],num_choices);

		ourSemitone = validTones[ourChoice];
		newNoteMIDI = rootNote.noteNumMIDI + ourSemitone - 1;
		Note newNote = Note(newNoteMIDI);

		return newNote;

	}

	Note getRandomNoteMutate() {

		//Get a random note from the scale but also randomly assign it an octave within
		//the allowable range if the option is set to do so. 

		int ourSemitone;
		int newNoteMIDI;
		int ourChoice;

		int num_choices = validToneWeights.size();
		ourChoice = weightedRandom(&validToneWeights[0],num_choices);

		ourSemitone = validTones[ourChoice];
		newNoteMIDI = rootNote.noteNumMIDI + ourSemitone - 1;
		Note newNote = Note(newNoteMIDI);

		if (mutate_octave_option) {
			int octaveRange = maxOctaveOffsetUp + maxOctaveOffsetDown + 1;
			int octavePos = std::rand() % octaveRange;
			int thisOctave = baseOctave + octaveOffset - maxOctaveOffsetDown + octavePos;
			newNote.octave = thisOctave;
			newNote.setNoteNumMIDI();
			newNote.setVoltage();
		}

		return newNote;

	}

};


struct ProteusPlusWidget : ModuleWidget {
	ProteusPlusWidget(ProteusPlus* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ProteusPlus.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.325, 18.528)), module, ProteusPlus::POT1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.325, 39.499)), module, ProteusPlus::POT2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.848, 28.726)), module, ProteusPlus::POT3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.848, 49.409)), module, ProteusPlus::POT4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.951, 18.717)), module, ProteusPlus::POT5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.951, 39.642)), module, ProteusPlus::POT6_PARAM));	
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.951, 60.605)), module, ProteusPlus::POT7_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.511, 83.737)), module, ProteusPlus::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.511, 97.813)), module, ProteusPlus::CV2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.612, 83.737)), module, ProteusPlus::CV3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.612, 97.813)), module, ProteusPlus::CV4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.743, 83.737)), module, ProteusPlus::CV5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.743, 97.813)), module, ProteusPlus::CV6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.911, 83.737)), module, ProteusPlus::CV7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.911, 97.813)), module, ProteusPlus::CV8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.511, 111.889)), module, ProteusPlus::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.612, 111.889)), module, ProteusPlus::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.743, 111.889)), module, ProteusPlus::AUDIOOUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.911, 111.889)), module, ProteusPlus::AUDIOOUT2_OUTPUT));

		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(18.6, 18.5)), module, ProteusPlus::LED1_LIGHT_RED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(23.6, 18.5)), module, ProteusPlus::LED2_LIGHT_RED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(28.6, 18.5)), module, ProteusPlus::LED3_LIGHT_RED));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(33.6, 18.5)), module, ProteusPlus::LED4_LIGHT_RED));
		addChild(createLightCentered<SmallLight<RedGreenBlueLight>>(mm2px(Vec(48, 5)), module, ProteusPlus::LEDX_LIGHT_RED));

		addParam(createParamCentered<VCVButton>(mm2px(Vec(25.85, 66.40)), module, ProteusPlus::BUTTON1_PARAM));

		addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(7.756, 59.027)), module, ProteusPlus::SWITCH1_PARAM));
		addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(7.756, 66.507)), module, ProteusPlus::SWITCH2_PARAM));
	}

	void appendContextMenu(Menu* menu) override {
		ProteusPlus* module = dynamic_cast<ProteusPlus*>(this->module);

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexSubmenuItem("Mutation can tranpose notes by octave", {"Off","On"},
			[=]() { return module->mutate_octave_option;},
			[=](int option) {module->mutate_octave_option = option;}
		));

		menu->addChild(createIndexSubmenuItem("Lock CV In Behavior", {"0-2","0-1","0-1-2","Hi/Low"},
			[=]() { return module->lock_option;},
			[=](int option) {module->lock_option = option;}
		));
	}
};


Model* modelProteusPlus = createModel<ProteusPlus, ProteusPlusWidget>("ProteusPlus");