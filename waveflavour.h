#include "maximilian.h"

#define TABLE_LEN 512
#define GENEROUS_TABLE_LEN 514

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
        void startSin();
        
        void setReverseParams(int dRev, int fRev);
        void setInvertParams(int dInv, int fInv);
        
        void invert();
        void reverse();
        void swap(WaveTable* other, int c);
        
};

class Voice {
    public :
        WaveTable* table;
        PhaseCounter p, offsetter;
        float volume;
        
        int note;
        
        void start(WaveTable* table, float freq);
        void setPitch(int note);
        double next();
};
        
class Sequencer {
    public :
        PhaseCounter tick;
        PhaseCounter noteTrigger;
        int len;
        int* notes;
        int currentNote;
        bool trigger;
        void start(int l, int* n, int speed);
        void step();
        void advanceNote();
};

