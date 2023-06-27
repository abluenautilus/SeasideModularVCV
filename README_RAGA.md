## Raga Module 2.5.2

Raga is a sitar simulator with 19 synthesized strings. 

A trigger into the STRUM input or a click of the related button will strum the strings of the sitar. 

***Sitar strings***

The melody is plucked on the *main string*. Control the main string level with the slider and/or the CV in. The sitar main string can be bent to change the pitch or the string can be fretted. This bending technique is called *meend*. Put the "meend" switch to the up position to enable meend mode. In meend mode, any time the pitch is changed without a new STRUM, the pitch will be bent. 

The next string is the *jora*, which is always tuned to the root note (Sa) and provides a drone sound when strummed. 

The *second* and *third* strings can be tuned to provide an underlying chord or to enhance the drone effect. Sometimes they are both tuned to a perfect fifth (Pa) but may be retuned depending on the raga or piece being played. 

The *taraf* strings lie underneath the main strings and are not plucked directly (with one exception) but rather ring sympathetically. When the main string is played at the frequency of one of the tarafs, it will ring out. There are 13 taraf strings. They can be enabled or disabled by clicking on the buttons in the "Sympathetic Tarafs" section. Typically you will enable the tarafs that match the scale you are playing, and leave the others disabled. 

The *chikari* strings are both tuned to Sa an octave apart and are strummed downward with a separate motion to create rhythmic patterns. To strum the chikaris, send a trigger to the CHIK input or press the associated button. 

The relative levels of the strings can be adjusted in the STRING MIX section.

***String Mix*** section

Control the levels of the various strings by moving the faders or sending in CV. Any CV will add with the fader to produce the level. The light on the fader represents the sum of its position and any incoming CV. When the faders are all the way off, the string is completely disabled and will not be processed. Disabling strings with the faders can be a way to reduce CPU usage. 

***Meend switch***

"Meend" is the technique of bending the main sitar string to reach higher notes without fretting. Unlike on a guitar, where you might be able to bend 1 or 2 semitones, a sitar is able to bend up to 5 or 6 semitones. To acheive this effect on Raga, slide the meend switch to the up position. When the meend switch is in the up position, any time the pitch input is changed *without a new STRUM*, the pitch will be bent instead of plucked. This is a kind of portamento/glide/slew effect. 

***Sympathetic Tarafs*** section

There are 13 tarafs. Click each button to enable or disable a taraf string. When disabled, the taraf will not ring sympathetically even if the main string matches its tuning. The buttons will glow when a corresponding taraf string is ringing. 

***Taraf Strum*** section

You can directly pluck the tarafs by running your finger across them. This is typically done while the other strings are silent, at the beginning of a piece. Send in a trigger or press the button to strum your finger across all of the enabeled tarafs. You can alter the selection of tarafs to change the feel of the set of ringing sympathetic tones, or to reduce CPU usage. 

***Tuning section*** section

You can change the root tuning of the sitar by adjusting the Main Sa tuning knob or sending in 1V/OCT signal into the input. This will retune all of the strings on the instrument to match the new root note. 

You can independently re-tune the second and third strings in a similar manner. 

***CPU usage***

Since Raga is synthesizing 19 strings simultaneously, CPU usage can be relatively high. This can be reduced by disabling strings in the string mix section (turning their faders all the way down), and also by disabling individual taraf strings by pressing the buttons. 