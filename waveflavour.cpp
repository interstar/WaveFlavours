#include "waveflavour.h"
#include "maximilian.h"
#include <math.h>
#include <stdio.h>


convert mtof;

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

void WaveTable::startSin() {
    startCommon();
    float a,da;
    a=0;
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


