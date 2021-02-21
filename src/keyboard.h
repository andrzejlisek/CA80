#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "eden.h"
#include "binary.h"
#include <iostream>

class Keyboard
{
public:
    bool NewCA80 = false;
    Keyboard();
    bool KeyPressed[24];
    void KeyPress(int N);
    void KeyRelease(int N);



    uchar GetDevState(uchar PortValue);
};

#endif // KEYBOARD_H
