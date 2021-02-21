#ifndef SOUND_H
#define SOUND_H

#include <iostream>
#include "eden.h"

#define PI2 6.283185307179586476925286766559
#define PI1 3.1415926535897932384626433832795

using namespace std;

class Sound
{
public:
    Sound();
    ~Sound();
    void Trigger();
    void Clock();
    void Reset();
    void EmuStart();
    void EmuStop();
    int GetSample(int Volume);
    long TriggerTime;
private:
    long Pulse1 = 0;
    long Pulse2 = 0;
    double Phase;
    double PhaseDelta;
};

#endif // SOUND_H
