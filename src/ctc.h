#ifndef CTC_H
#define CTC_H

#include "eden.h"
#include "binary.h"
#include <iostream>

class CTC
{
public:
    bool CTC0Exists = false;
    llong CTC0Time = 0;
    llong CTC0TimeC = 0;
    CTC();
    void Reset();
    void Set(uchar ChN, uchar Val);
    uchar Get(uchar ChN);
    void Trigger(uchar ChN);
    void Clock();
    bool Interrupt();
    void InterruptReset();
    uchar InterruptAddr();
    uchar CCR_7[4];
    uchar CCR_6[4];
    uchar CCR_5[4];
    uchar CCR_4[4];
    uchar CCR_3[4];
    uchar CCR_2[4];
    uchar CCR_1[4];
    uchar TCR[4];
    uchar DownCounter[4];
    int PrescalerCounter[4];
    int PrescalerPeriod[4];
    uchar InterruptFlag[4];
    uchar InterruptAddr_[4];
private:
};

#endif // CTC_H
