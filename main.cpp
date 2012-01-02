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

int notes[8][2] = { {50,96},
                    {53,48},
                    {55,96},
                    {56,48},
                    {57,96},
                    {59,48},
                    {60,48},
                    {61,48} };

int chordOffs[4]={0,3,7,10};                    
int bassNotes[4][2] = { {50,192},
                   {58,192},
                   {56,192},
                   {61,192} };


maxiEnv env, envBass;
maxiDelayline delay;

int norm[1]={0};

void setup() {//some inits

    printf("start\n");
    wave1.startSin();
    wave2.startSin();
    wave1.setReverseParams(0,9987);
    wave1.setInvertParams(0,701);
    
    wave3.startRamp();
    wave4.startSin();
    wave3.setReverseParams(1,1001);
    wave4.setInvertParams(1,3000);
        
    swpTrigger.start(0,1001);
    swp=0;

    swpTrigger2.start(0,1001);
    swp2=0;
    
    v1.start(&wave1,1,norm,50);
    v1.setPitch(0);
    v1.volume = 0.8;
    v1.offsetter.dx = 0.02;

    v2.start(&wave3,4,chordOffs,46);
    v2.setPitch(0);
     
    seq.start(8,notes,300);
    bass.start(4,bassNotes,300);
    seq.advanceNote();
    bass.advanceNote();
 
    flag = false;
}

void play(double *output) { 
    if (flag) { setup(); }

    if (seq.trigger) {
        int nv = seq.currentNote;
        v1.setPitch(nv);
    }    

    
    if (bass.trigger) {
        int nv = bass.currentNote;
        v2.setPitch(nv);
    }    


	double o,o2;
	o = v1.next();
	o = env.ar(o, 0.05, 0.9975, 5000, seq.trigger);
	//o = delay.dl(o, 20000, 0.9);
	

	o2 = v2.next();
	o2 = envBass.ar(o2, 0.05, 0.999975, 5000, bass.trigger);
	output[0]=(o+o2)/2;
	output[1]=(o+o2)/2;	

    wave1.reverse();
    wave1.invert();

    wave3.reverse();
    wave3.invert();

    swpTrigger.next();    
    if (swpTrigger.wrapped()) {

        wave1.swap(&wave2,swp);
        swp++;
        if (swp > wave1.len) {
            swp=0;    
        }
    }

    swpTrigger2.next();
    if (swpTrigger2.wrapped()) {
        wave3.swap(&wave4,swp2);
        swp2++;
        if (swp2 > wave3.len) {
            swp2=0;    
        }
    }

    seq.step();
    bass.step();

}
