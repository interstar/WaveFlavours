#include "waveflavour.h"

#define MAX_INSTRUMENTS 10

typedef struct {
    int note;
    int duration;
} Note;


class Sequencer {
    public :
        PhaseCounter tick;
        PhaseCounter noteTrigger;
        int len,speed;
        Note* notes;
        int currentNote,currentDuration;
        
        bool trigger;
        void start(int l, int n[][2], int speed);
        void step();
        void advanceNote();
};


/*
class Score {
    int noSequences;
    Sequencer sequences[MAX_INSTRUMENTS];
    Instrument instruments[MAX_INSTRUMENTS];
    
    void setup();
    void addSequence(int len, int n[][2], int speed);
    Instrument* addInstrument(int waveform1, int waveform2, int noVoices, int* pitchOffsets, int seqNo);
    void play();
}
*/
