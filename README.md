# Seaside Modular VCV Rack Plugin

## Proteus Module

Proteus is a generative sequencer. It creates a melody, then loops that melody for a while, until it gets bored and creates a new one. While it's looping the melody, it may transpose by octave or mutate individual notes as controlled by the knobs or CV.

A video demonstration can be found here: 

https://www.youtube.com/watch?v=XAcYI5NUyIQ

### Controls

**Length** Sets the sequence length from 1 to 32 steps. You can change the sequence length on the fly without disturbing the melody. For instance, you can make it shorter and then longer again without changing the underlying melody. There is always a 32 step sequence in memory whether you are looping the whole thing or not. 

**Scale** Sets the scale that notes are drawn from. Currently scales are, from clockwise to counter-clockwise:
Major
Natural Minor
Harmonic Minor
Major Pentatonic
Minor Pentatonic
Mixolydian

**Lambda** This controls the probability function that determines how likely Proteus is to create a brand new melody. When the number of repeats reaches lambda, there is a 50% chance of a new melody. Proteus uses a Poisson cumulative density function. The shape of this function means that probability will stay very low for several repetitions, but will begin to climb rapidly as you approach lambda. The brightness of the LEDs indicates the current probability of a change; they will start out dim and get brighter every loop. 

***Gate*** This is the length of the gate output as a fraction of the incoming clock time (average trigger-to-trigger time).

***Density*** This controls the density of the melodies. As you turn this knob towards 100, the probability of rest notes decreases to 0 and all the steps will be filled with notes. 

***Octave** Controls octave transposition probability. 

***Mutate*** Controls the probability of random note mutations. 

***MODE switch*** When in the leftmost position, Proteus always has some chance of creating a new melody based on the underlying probability function that increases with each loop. When switched to the center position, that probability will freeze in place and Proteus will not ever generate a new melody. However, it will continue octave shifts and note mutations. Shifting this switch all the way to the right towards the lock icon both locks in the current melody and turns off octave shifts and note mutations. This is akin to turning the knob of a Turing Machine fully clockwise. 

***DIR switch*** Controls which direction octave transpositions will move. In the center positions, shifts upwards and downards are equally likely. Proteus will never transpose a melody more than two octaves away from its starting point (based on C4). If you push the switch to the left, the octave transpositions will only go downwards. If you shift the switch all the way to the right, octave transpositions will only go upwards. This is useful if you find the melody is too high or too low and want to force to move in a certain direction. 

**Center button** Pushing the center button will cause Proteus to immediately generate a new melody. 

### Inputs

Len, Lambda, Density, Mutate, Scale, Gate, and Octave are CV controls for their respective parameters. 

***Clock*** This is the clock input. Each trigger incoming on this port advances the sequence one step. 

***NEW*** A trigger on this channel will cause Proteus to immediately generate a new melody. 

### Outputs

***V/OCT*** Volt per octave out. 

***Gate*** Gate output
