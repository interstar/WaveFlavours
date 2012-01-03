#include "maximilian.h"

#define TABLE_LEN 512
#define GENEROUS_TABLE_LEN 514
#define MAX_INSTRUMENTS 10

#define SIN -100
#define RAMP -99

typedef struct {
    int note;
    int duration;
} Note;


class PhaseCounter {
    public :
        float x, oldX, dx, max;
        void start(float d, float m);
        int next();
        bool flipped();
        bool wrapped();
};


class WaveTable {
    public : 
        int len;
        double wave[GENEROUS_TABLE_LEN];
        
        int iRev;
        PhaseCounter revTrigger;
        
        int iInv;
        PhaseCounter invTrigger;
        
        void startCommon();
        void startRamp();
        void startSin(float a);
        
        void setReverseParams(int dRev, int fRev);
        void setInvertParams(int dInv, int fInv);
        
        void invert();
        void reverse();
        void swap(WaveTable* other, int c);
        
};

class Voice {
    public :
        WaveTable* table;
        PhaseCounter p[10], offsetter;
        int pitchOffsets[10];
        float volume;
        
        int noVoices,note,globalPitchOffset;
        
        
        void start(WaveTable* table, int noVoices, int* pitchOffsets, float freq);
        void setPitch(int note);
        double next();

};

class Instrument {
    // Voice plus its wavetables
    public:
        WaveTable table1, table2;
        Voice voice;
        maxiEnv env;
        PhaseCounter swapTrigger;
        int swapIndex;
        float envAttack, envRelease, envSustain;
            
        void start(int waveform1, int waveform2, int noVoices, int* pitchOffsets);
        void setParams(int revInc, int reverse, int invInc, int invert, int swapInc, int swap, float dPhase, int globalPitchOffset);
        void setEnv(float attack, float release, float sustain);
        double next(int pitch, int trigger);
};

        
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
