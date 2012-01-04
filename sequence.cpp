#include "sequence.h"
#include <math.h>
#include <stdio.h>

#define MAX_INSTRUMENTS 10

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
    tick.start(1,speed*currentDuration);
}


