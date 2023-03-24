## Proteus Module v2.4.2

Proteus is a generative sequencer. It creates a melody, then loops that melody for a while, until it gets bored and creates a new one. While it's looping the melody, it may transpose by octave or mutate individual notes as controlled by the knobs or CV.

A video demonstration can be found here (note it needs to be updated with the latest panel): 

https://www.youtube.com/watch?v=XAcYI5NUyIQ

Announcement and discussion thread: 

https://community.vcvrack.com/t/proteus-from-seaside-modular-a-new-generative-sequencer-v-2-1-0-released/18298/42


### Controls

**Length** - Sets the sequence length from 1 to 32 steps. You can change the sequence length on the fly without disturbing the melody. For instance, you can make it shorter and then longer again without changing the underlying melody. There is always a 32 step sequence in memory whether you are looping the whole thing or not. 

**Scale** - Sets the scale that notes are drawn from. Currently scales are, from clockwise to counter-clockwise:
Major
Natural Minor
Harmonic Minor
Major Pentatonic
Minor Pentatonic
Dorian
Mixolydian
Chromatic
Tuning (All C's, for tuning your oscillators)
Bhairavi
Ahir Bhairav

**Lambda** - This controls the probability function that determines how likely Proteus is to create a brand new melody. When the number of repeats reaches lambda, there is a 50% chance of a new melody. Proteus uses a Poisson cumulative density function. The shape of this function means that probability will stay very low for several repetitions, but will begin to climb rapidly as you approach lambda. The brightness of the LEDs indicates the current probability of a change; they will start out dim and get brighter every loop. 

**Gate** - This is the length of the gate output as a fraction of the incoming clock time (average trigger-to-trigger time).

**Density** - This controls the density of the melodies. As you turn this knob towards 100, the proportion of rest notes decreases to 0 and all the steps will be filled with notes. Density is changed dynamically; every time the parameter changes, each note's probabilty of becoming a rest is independently re-calculated according to this parameter. Notes appear and disappear in the same order, so that if you increase density and then decrease it, you will come back to the exact same sequence. However, if you change the length of the sequence, the "disappearing order" of the notes will have to be shuffled to adjust to the new sequence length. 

**Octave** - Controls octave transposition probability. 

**Mutate** - Controls the probability of random note mutations. 

**MODE switch** - When in the leftmost position, Proteus always has some chance of creating a new melody based on the underlying probability function that increases with each loop. When switched to the center position, that probability will freeze in place and Proteus will not ever generate a new melody. However, it will continue octave shifts and note mutations. Shifting this switch all the way to the right towards the lock icon both locks in the current melody and turns off octave shifts and note mutations. This is akin to turning the knob of a Turing Machine fully clockwise. 

**RANGE switch** - Controls the range of octave transpositions. The leftmost position turns off octave transpositions and is equivalent to setting the Octave parameter to 0. The middle position allows for plus or minus 1 octave transposition from the base octave (4). The rightmost position allows for plus or minus 2 octaves in each direction from the base octave.

**Center button (New sequence)** - Pushing the center button will cause Proteus to immediately generate a new melody. 

### Inputs

Len, Lambda, Density, Mutate, Gate, and Octave are CV controls for their respective parameters. 

**CLOCK** - This is the clock input. Each trigger incoming on this port advances the sequence one step. 

**LOCK** - A trigger into this jack will change the lock mode according to the menu option you have set: 
    *0-2 (Default):* Trigger toggles between modes 0 and 2
    *0-1:* Trigger toggles between modes 0 and 1
    *0-1-2:* Trigger cycles through the three modes
    *Hi/Low:* As long as the trigger is high, the sequence will be totally locked (i.e., mode 2), and when it is low, the  sequence will be totally unlocked (mode 0)

**NEW** -  trigger on this channel will cause Proteus to immediately generate a new melody. 

**RESET** - Resets the sequence to its first step. 

### Outputs

**V/OCT** - Volt per octave out. 

**GATE** - Gate output

The light in the upper right corner of Proteus indicates if it has detected the Proteus Expander. See the manual for the Proteus Expander for more information. 