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
    if (x < 0) { x = 0; }
    if (x > max) { x = 0; }
    x = x + dx;
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

void Voice::start(WaveTable* table, float freq) {
    this->table = table;
    p.start(freq,TABLE_LEN);
    offsetter.start(0,TABLE_LEN);
    volume=1;
}

void Voice::setPitch(int n) {
    note = n;
    float freq = mtof.mtof(n);
    float x = 44100/freq;
    p.dx=TABLE_LEN/x;
}

double Voice::next() {
    int c = p.next();
    int offset = offsetter.next();
    int oset = (c+offset)%TABLE_LEN;
    return volume*((table->wave[c] + table->wave[oset])/2);
}


void Sequencer::start(int l, int* n, int speed) {
    tick.start(1,speed);
    noteTrigger.start(1,l);
    this->notes = n;
    trigger=1;
    currentNote = notes[0];
}
        
void Sequencer::step() {
    tick.next();
    if (tick.wrapped()) {
        int c = noteTrigger.next();
        currentNote = notes[c-1];
        trigger = true;
    } else {
        trigger = false;
    }
}

void Sequencer::advanceNote() {
    int c = noteTrigger.next();
    currentNote = notes[c-1];
}


