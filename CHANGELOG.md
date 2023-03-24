# Changelog

## v.2.4.2 - March 2023

• Fixes loading and saving of sequences

• Density is now accurately maintained when the sequence changes in length

• Sequence light positions have been fixed

• Lights no longer advance during rest, instead light 1 blinks orange

• Rest and Transpose adjustments go away if Px is not connected to Proteus

## v.2.4.1 - March 2023

• Adds v/oct tuning for Jawari

• Adds tuning, bhairavi, and ahir bhairav scales. Tuning mode just outputs C's and is for tuning your oscillators. 

## v.2.4.0 - March 2023

• Jawari has been redesigned and uses significantly less CPU

• Jawari now has a reset input

## v2.3.0 - 3 Jan 2023

• Proteus has been updated to work with the expander module ProteusX

• New module Jawari, a tanpura simulator

## v2.2.0 - 18 Oct 2022

• Density now affects the overall proportion of notes in the sequence that are present instead of a probability applied to each note individually. This means that if Density is set to 50%, you will always have exactly 50% of the notes present. This also solves the problem of sequences with low Density ending up empty. There will always be at least one note on in the sequence. 

• Live density changes are made in a fixed order. That means that if you start with 50% of the notes, and turn Density up to 100%, the notes will fill in, and then when you move the knob back to 50% the notes will disappear in the order that they came in, such that when you get back to 50% you will have exactly the same sequence you started with. 

• There is now a menu option for mutated notes to jump octaves, constrained by the octave range setting. In other words, with this option on, and octave range set to 1, a mutated note may appear anywhere from 1 octave above to 1 octave below the sequence. 

• Lock CV input has replaced the Scale CV input. A trigger into this jack will change the lock mode according to the menu option you have set: 
    *0-2 (Default):* Trigger toggles between modes 0 and 2
    *0-1:* Trigger toggles between modes 0 and 1
    *0-1-2:* Trigger cycles through the three modes
    *Hi/Low:* As long as the trigger is high, the sequence will be totally locked (i.e., mode 2), and when it is low, the  sequence will be totally unlocked (mode 0)


## v2.1.0 - 21 Sept 2022

• Added RESET jack

• Density changes are now dynamic

• The bottom switch now controls octave range instead of octave change direction. Left position: 0, no octave transpositions; Middle position: 1, can transpose up or down 1 octave from the starting position; 2, can transpose up or down 2 octaves from the starting positions (this was the original setting)

• Added Dorian and Chromatic scales

• Proteus now saves its melody on save/autosave

• Improved state persistence in general

• Improved gate length handling (hopefully addresses bug where gate stays high on startup)


