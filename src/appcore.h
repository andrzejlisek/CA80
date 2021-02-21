#ifndef APPCORE_H
#define APPCORE_H

#include "eden.h"
#include "cpumem.h"
#include "display.h"
#include "keyboard.h"
#include "i8255_sys.h"
#include "i8255_usr.h"
#include <map>
#include "sound.h"
#include "taperecorder.h"
#include <string>
#include <iostream>
#include "extdatetime.h"
#include "music.h"

class AppCore
{
public:
    AppCore();
    ~AppCore();

    CpuMem * _CpuMem;
    Display * _Display;
    Keyboard * _Keyboard;
    I8255_SYS * _I8255_SYS;
    I8255_USR * _I8255_USR;
    Sound * _Sound;
    TapeRecorder * _TapeRecorder;
    ExtDateTime * _ExtDateTime;
    Music * _Music;

    map<int,int> KeyMap;
    string VisKey[24];
    bool SoundAudible;
    int SoundVolume;
    int MusicVolume;
    void Reset();

    int TimerPeriod;
    int SoundBufSize;
    int SoundChunkSize;
};

#endif // APPCORE_H
