#ifndef I8255_SYS_H
#define I8255_SYS_H

#include "i8255.h"
#include "keyboard.h"
#include "display.h"
#include "taperecorder.h"

class I8255_SYS : public I8255
{
public:
    I8255_SYS();
    ~I8255_SYS();
    void Update8255();

    Keyboard * _Keyboard;
    Display * _Display;
    TapeRecorder * _TapeRecorder;
};

#endif // I8255_SYS_H
