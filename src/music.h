#ifndef MUSIC_H
#define MUSIC_H

#include <cmath>

#define PI2 6.283185307179586476925286766559

class Music
{
public:
    Music();
    ~Music();
    bool IsActive;
    double Phase;
    double PhaseDelta;
    int GetSample(double Volume);
    void SetPitch(double Val);
};

#endif // MUSIC_H
