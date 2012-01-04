#include "maximilian.h"
#include "waveflavour.h"
#include "sequence.h"

#include <math.h>
#include <stdio.h>

bool flag = true;


/*
// Sublime Loop (See http://www.sublimeloop.com/ for details )
int notes[64] = {50,53,50,53, 52,55,52,55, 48,52,48,52, 50,54,50,54, 
                 50,53,50,53, 52,55,52,55, 48,52,48,52, 50,54,50,54,
                 50,53,50,53, 52,55,52,55, 48,52,48,52, 50,54,50,54,                 
                 50,53,50,53, 52,55,52,55, 57,57,57,57, 57,57,57,57
};

int bassNotes[16] = {46, 48, 45, 42, 
                    46, 48, 45, 42, 
                    46, 48, 45, 42, 
                    46, 48, 50, 50 
};
*/

// new test sequence

// notes in {pitch,duration} format
int notes[11][2] = { {50,96},
                    {53,48},
                    {55,96},
                    {56,48},
                    {57,96},
                    {59,48},
                    {60,48},
                    {61,48},
                    {62,96}, 
                    {67,48},
                    {65,96}
                    };

// these are chord offsets, they're used for the Voice which is playing a base-note plus these offsets
int chordOffs[4]={0,3,7,10};                    

// the actual sequence of base-notes for the chord voice
int chordNotes[4][2] = { {50,192},
                   {52,192},
                   {57,192},
                   {52,192} };

// a "high-hat" pattern
int highNotes[16][2] = { {100,24},
                       {100,24},
                       {100,24},
                       {88,24},
                       {100,24},
                       {100,24},
                       {100,24},
                       {88,24},
                       {100,24},
                       {100,24},
                       {100,24},
                       {88,24},
                       {100,24},
                       {100,24},
                       {88,24},
                       {100,24} };


Sequencer seq, chords, hat;

Instrument inChords, inSolo, inHat;

int norm[1]={0};

void setup() {
    //some inits

    printf("start\n");
    
    inChords.start(SIN,RAMP,4,chordOffs); // creates an Instrument with Sin and Ramp waves, 4 actual voices, spaced with chordOffs
    inChords.setParams(1,9987,1,7001,1,90001,0.02,0); // sets up the sound parameters for this Instrument 
    inChords.setEnv(0.5, 0.999975, 5000); // sets envelope for the Instrument
    inChords.voice.volume = 1.1; // sets volume
        
    inSolo.start(SIN,SIN,1,norm); // single voiced Instrument, made with 2 Sin waves
    inSolo.setParams(0,9987,1,7001,0,1001,0,-12); // sets up the sound parameters for this Instrument
    inSolo.setEnv(0.05, 0.909975, 5000); // sets envelope for the Instrument ... etc.
    inSolo.voice.volume = 0.5;
    
    inHat.start(SIN,RAMP,1,norm);
    inHat.setParams(1,48,1,19999,1,1000,0.1,0);
    inHat.setEnv(0.01, 0.99, 1000);
        
    // sets up the three sequences 
    seq.start(11,notes,250);
    chords.start(4,chordNotes,250);
    hat.start(16,highNotes,250);
    
    // push the sequences to the beginning.
    seq.advanceNote();
    chords.advanceNote();
    hat.advanceNote();
 
    flag = false;
}

void play(double *output) { 
    if (flag) { setup(); }

    double o,o2,o3;
    o = inChords.next(chords.currentNote,chords.trigger);
    o2 = inSolo.next(seq.currentNote,seq.trigger);
    o3 = inHat.next(hat.currentNote,hat.trigger);
    
	output[0]=(o+o2+o3)/3;
	output[1]=(o+o2+o3)/3;	
	
    seq.step();
    chords.step();
    hat.step();
}
