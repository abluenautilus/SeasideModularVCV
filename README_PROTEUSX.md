## Proteus Expander Module v2.4.2

ProteusX adds functionality to the Proteus generative sequencer. ProteusX doesn't do anything on its own. 

### Connection

ProteusX should be placed immediately to the right side of a Proteus module. When a connection is made, the lights on the upper left hand side of ProteusX and on the upper right hand side of Proteus will illuminate. 

When Proteus becomes disconnected from ProteusX, the extra parameters from pX, Rest and Transpose, will no longer affect Proteus. If you reconnect pX, they will once again take effect. 

### Pattern bank

The pattern bank is for saving and re-loading patterns that Proteus creates. If Proteus makes a melody that you like and wish to return to, you can save it in one of the five slots. Once melodies are saved in the slots, you either recall melodies using the LOAD buttons, or you can use triggers into the Next, Prev, and Rand inputs to change among the stored patterns. 

To prevent accidental overwrite, you cannot save a pattern into a slot which already has a pattern saved. To free a slot, you must first press the CLEAR button to clear the slot. 

When you load a pattern from pX, the LENGTH and DENSITY parameters from Proteus will take effect on that sequence. In other words, if you saved a pattern when it was 16 notes, but load it back when Proteus is set to 8 notes, you will hear the shortened, 8 note version when you load it back in. But don't worry, all of the original notes are there and can be heard by making the sequence longer once again. Note that the other knobs on Proteus don't affect loaded sequences, since those only affect the generation of new sequences. For instance, if you saved a pattern in a major scale, but load it back in when Proteus is set to minor, you will still hear the original major scale notes. 

### Additional controls

***Transpose***

By default, Proteus makes its melodies in the key of C. If you wish to use a different root note, you can use the transpose knob or CV input to transpose all of the melodies by semitone. 

***Rest***

This knob and CV input will play silent rest notes after the completion of each loop. The first light on Proteus will blink orange when it is resting. 