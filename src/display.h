#ifndef DISPLAY_H
#define DISPLAY_H

#include "eden.h"
#include "binary.h"
#include <string>
#include <sstream>
#include <iostream>

class Display
{
public:
    Display();
    string GetDisplay();
    void Update(uchar Segment, uchar Value);
    void Clock();
    void Reset();
private:
    uchar Buf[8];
    uchar Buf0[8];
    int Refresh = 65000;
    int DisplayTimer[8][8];
};

#endif // DISPLAY_H
