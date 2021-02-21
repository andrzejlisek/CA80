#include "sound.h"

Sound::Sound()
{
    Reset();
}

Sound::~Sound()
{
}

void Sound::EmuStart()
{
}

void Sound::EmuStop()
{
}


void Sound::Trigger()
{
    Pulse2 = Pulse1;
    Pulse1 = TriggerTime;
    if (Pulse2 > 0)
    {
        double T = Pulse1 - Pulse2;
        PhaseDelta = 0;
        if (T > 0)
        {
            //PhaseDelta = PI2 / ((48000.0 / 4000000.0) * T);
            PhaseDelta = (PI2 * 4000000.0) / (48000.0 * T);
        }
    }
}

void Sound::Clock()
{
    if (Pulse1 > 0) { Pulse1--; }
    if (Pulse2 > 0) { Pulse2--; }
}

void Sound::Reset()
{
    Phase = 0;
    PhaseDelta = 0;
    Pulse1 = 0;
    Pulse2 = 0;
}

int Sound::GetSample(int Volume)
{
    if ((Pulse1 > 0) & (Pulse2 > 0))
    {
        Phase = Phase + PhaseDelta;
        if (Phase >= PI2)
        {
            Phase -= PI2;
        }

        if (Phase < PI1)
        {
            return (Volume);
        }
        else
        {
            return (0 - Volume);
        }
    }
    else
    {
        return 0;
    }
}
