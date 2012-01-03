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

int PhaseCounter::next() {
    oldX = x;
    x = x + dx;

    if (x < 0) { x = 0; }
    if (x >= max) { x = 0; }
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
    if (waveform1 == SIN) {
        table1.startSin(0);
    } else {
        table1.startRamp();
    }
    if (waveform2 == SIN) {
        table2.startSin(3.1415/4);
    } else {
        table2.startRamp();
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
    
// __Sequencer______________________________________________________________________________
//
void Sequencer::start(int l, int n[][2], int s) {
    printf("Starting sequencer\n");
    len = l;
    speed = s;
    printf("Len: %d, Speed: %d\n",len,speed);
    
    notes = new Note[len];
    for (int i=0;i<len;i++) {
        notes[i].note = n[i][0];
        notes[i].duration = n[i][1];
        printf("%d) %d,%d\n",i,notes[i].note,notes[i].duration);
    }

    trigger=1;
    currentNote = notes[0].note;
    currentDuration = notes[0].duration;

    noteTrigger.start(1,len);
    noteTrigger.x = noteTrigger.max;
    tick.start(1,speed*currentDuration);
    printf("Sequencer started with speed: %d, note: %d and duration: %d, %d\n",speed,currentNote,currentDuration,tick.max);
}
        
void Sequencer::step() {
    tick.next();
    if (tick.wrapped()) {
        printf("*");
        advanceNote();        
        trigger = true;
    } else {
        trigger = false;
    }
}

void Sequencer::advanceNote() {
    int c = noteTrigger.next();
    currentNote = notes[c].note;
    currentDuration = notes[c].duration;
    printf("%d: %d, %d\n",c,currentNote,currentDuration);
    tick.start(1,speed*currentDuration);
}


