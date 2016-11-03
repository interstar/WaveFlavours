## WaveFlavours


### A simple wavetable synthesis technique

The aim of WaveFlavours is to produce interesting, evolving sounds with very little processing power. Voices are defined in terms of two wave-tables that start with two different waves (eg. a sin and ramp) and the main transformation of the sound is to gradually swap corresponding sample points between the two. Table 1 is actually played, while table 2 is a store. Over a period of time, the sin in table 1 will have swapped places with the ramp in table 2. The swapping will continue, reverting sin into the active wave again. Intermediate positions of half-sin half-ramp will obviously be richer in harmonics.

Further transformations available include swapping points from the beginning and end of the same wave. And inverting one point at a time.

Voices scan the wave-table, playing a mix of the scan point and the scan point + an offset. By varying the offset gradually you can add a phaser-type effect to the sound. While varying it at a higher frequency produces atonal FM-like clinks and clanks.

### Running

At the moment, the code is designed to work with the Maximilian project ( https://github.com/micknoise/Maximilian ). So start by cloning that project or my fork ( https://github.com/interstar/Maximilian ). Then copy all files from this project directly into the same directory. (Not elegant, but it will do for now. I'll work out something a bit tidier shortly, probably keeping Maximilian in a submodule.)

Follow the instructions for command-line compiling in the README of Maximilian. ( https://github.com/micknoise/Maximilian/blob/master/README )


