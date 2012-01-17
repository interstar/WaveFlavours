#include "waveflavour.h"
#include "maximilian.h"
#include <math.h>
#include <stdio.h>


convert mtof;

// __PhaseCounter__________________________________________________________________________
//
void PhaseCounter::start(float d, float m) {
    x=0;
    oldX = 0;
    dx = d;
    max = m;
}

bool PhaseCounter::flipped() {
    if ((int)oldX == (int)x) { return false; }
    return true;
}

bool PhaseCounter::wrapped() {
    if (oldX > x) { return true; }
    return false;
}

//  FIXME: Return phase accumulator as a float to enable interpolation.
int PhaseCounter::next() {
    oldX = x;
    x = x + dx;

    if (x < 0) { x = 0; }
    x = fmodf(x, max);
    return (int)x;
}



// __WaveTable_________________________________________________________________________
//
void WaveTable::startCommon() {
    len=TABLE_LEN;
    iRev = 0;
    iInv = 0;
    setReverseParams(0,1000);
    setInvertParams(0,1000);
}

void WaveTable::setReverseParams(int dr, int mr) {
    revTrigger.start(dr,mr);   
}

void WaveTable::setInvertParams(int di, int mi) {
    invTrigger.start(di,mi);    
}

void WaveTable::startRamp() {
    startCommon();
    for (int i=0;i<len;i++) {
        wave[i] = (float)i/(float)len;
    }    
}

void WaveTable::startSin(float a) {
    startCommon();
    float da;
    da = (3.1415*2)/len;
    for (int i=0;i<len;i++) {
        wave[i] = sin(a);
        a+=da;
    }    
}

void WaveTable::startSquare() {
    startCommon();
    for (int i=0;i<len;i++) {
        wave[i] = i < len/2?-1:1;
    }
}

void WaveTable::reverse() { 
    double tmp;
    revTrigger.next();
    if (revTrigger.wrapped()) {
        tmp = wave[iRev];
        wave[iRev] = wave[len-iRev];
        wave[len-iRev]=tmp;
        iRev++;  
        if (iRev>=len) { iRev=0; }      
    }
}

void WaveTable::invert() { 
    double tmp;
    invTrigger.next();
    if (invTrigger.wrapped()) {
        tmp = wave[iInv]+1;
        wave[iInv]=(2-tmp)-1;
        iInv++;
        if (iInv>=len) { iInv=0; }
    }
}

void WaveTable::swap(WaveTable* other, int c) {
    double tmp;
    tmp = wave[c];
    wave[c]=other->wave[c];
    other->wave[c]=tmp;
}

// __Voice_____________________________________________________________________
//
void Voice::start(WaveTable* table, int nv, int* pOffs, float freq) {
    this->table = table;
    noVoices = nv;
    for (int i=0;i<noVoices;i++) {
        p[i].start(freq,TABLE_LEN);
        pitchOffsets[i]=pOffs[i];    
    }
    offsetter.start(0,TABLE_LEN);
    volume=1;
    globalPitchOffset = 0;
}

float calculatePitch(int n) {
    float freq = mtof.mtof(n);
    float x = 44100/freq;
    return TABLE_LEN/x;
}

void Voice::setPitch(int n) {
    note = n;
    for (int i=0;i<noVoices;i++) {
        p[i].dx=calculatePitch(n+pitchOffsets[i]+globalPitchOffset);
    }
}

double Voice::next() {
    int offset = offsetter.next();
    double vTot=0;
    for (int i=0;i<noVoices;i++) {
        int c = p[i].next();
        int oset = (c+offset)%TABLE_LEN;
        vTot = vTot + ((table->wave[c]+table->wave[oset])/2);
    }        
    return volume*vTot/noVoices;
}

// __Instrument_____________________________________________________________________________
//
void Instrument::start(int waveform1, int waveform2, int noVoices, int* pitchOffsets) {
    switch (waveform1) {
        case SIN:
            table1.startSin(0);
            break;
        case RAMP:
            table1.startRamp();
            break;
        case SQUARE:
            table1.startSquare();
            break;
    }
    
    switch (waveform2) {
        case SIN:
            table2.startSin(0);
            break;
        case RAMP:
            table2.startRamp();
            break;
        case SQUARE:
            table2.startSquare();
            break;
    }
        
    voice.start(&table1, noVoices, pitchOffsets,0);
}

void Instrument::setParams(int revInc, int reverse, int invInc, int invert, int swapInc, int swap, float dPhase, int globalPitchOffset) {
    table1.setReverseParams(revInc,reverse);
    table1.setInvertParams(invInc,invert);
    voice.globalPitchOffset = globalPitchOffset;
    voice.offsetter.dx = dPhase;
    swapIndex = 0;
    swapTrigger.start(swapInc,swap);
}

void Instrument::setEnv(float attack, float release, float sustain) {
    envAttack = attack;
    envRelease = release;
    envSustain = sustain;
}

double Instrument::next(int pitch, int trigger) {
    if (trigger) {
        voice.setPitch(pitch);
    }
    double o = env.ar(voice.next(),envAttack,envRelease,envSustain,trigger);

    table1.reverse();
    table1.invert();
    swapTrigger.next();
    if (swapTrigger.wrapped()) {
        table1.swap(&table2,swapIndex);
    }
    swapIndex++;
    if (swapIndex > table1.len) {
        swapIndex = 1;
    }
    return o;
}
    

