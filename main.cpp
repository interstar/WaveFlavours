#include "maximilian.h"
#include "waveflavour.h"

#include <math.h>
#include <stdio.h>

WaveTable wave1, wave2, wave3, wave4;
Voice v1,v2,v3;

bool flag = true;

int swp,swp2;
PhaseCounter swpTrigger, swpTrigger2;

Sequencer seq, bass;

// Sublime Loop (See http://www.sublimeloop.com/ for details )
/*
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

// new test chords / seq
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

int chordOffs[4]={0,3,7,10};                    
int bassNotes[4][2] = { {50,192},
                   {52,192},
                   {57,192},
                   {52,192} };


Instrument inChords, inSolo;

maxiEnv env, envBass;
maxiDelayline delay;

int norm[1]={0};

void setup() {//some inits

    printf("start\n");
    
    inChords.start(SIN,RAMP,4,chordOffs);
    inChords.setParams(0,9987,1,7001,1,90001,0.02,0);
    inChords.setEnv(0.5, 0.999975, 5000);
    inChords.voice.volume = 1.1;
        
    inSolo.start(SIN,SIN,1,norm);
    inSolo.setParams(0,9987,1,7001,0,1001,0,-12);
    inSolo.setEnv(0.05, 0.909975, 5000);
    inSolo.voice.volume = 0.5;
         
    seq.start(11,notes,250);
    bass.start(4,bassNotes,250);
    seq.advanceNote();
    bass.advanceNote();
 
    flag = false;
}

void play(double *output) { 
    if (flag) { setup(); }

    double o,o2;
    o = inChords.next(bass.currentNote,bass.trigger);
    o2 = inSolo.next(seq.currentNote,seq.trigger);
	output[0]=(o+o2)/2;
	output[1]=(o+o2)/2;	
    seq.step();
    bass.step();
}
