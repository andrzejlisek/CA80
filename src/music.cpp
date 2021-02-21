#include "music.h"

Music::Music()
{
    Phase = 0;
    PhaseDelta = 0;
    IsActive = 0;
}

Music::~Music()
{

}

int Music::GetSample(double Volume)
{
    if (IsActive)
    {
        Phase += PhaseDelta;
        if (Phase >= PI2)
        {
            Phase -= PI2;
        }
    }
    double X = sin(Phase) * Volume;
    return X;
}

void Music::SetPitch(double Val)
{
    if (Val > 0)
    {
        //double Freq = 112640.0 / Val;
        //double Period = 48000.0 / Freq;
        //PhaseDelta = PI2 / Period;
        PhaseDelta = (PI2 * 112640.0) / (48000.0 * Val * 2);
    }
    else
    {
        PhaseDelta = 0;
    }
}
