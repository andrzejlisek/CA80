#ifndef I8255_H
#define I8255_H

#include "eden.h"
#include "binary.h"
#include <iostream>

class I8255
{
public:
    I8255();
    virtual ~I8255();
    void PortASet(uchar Val);
    void PortBSet(uchar Val);
    void PortCSet(uchar Val);
    void PortSSet(uchar Val);
    uchar PortAGet();
    uchar PortBGet();
    uchar PortCGet();

    void DevASet(uchar Val);
    void DevBSet(uchar Val);
    void DevCSet(uchar Val);
    void DevCHSet(uchar Val);
    void DevCLSet(uchar Val);
    uchar DevAGet();
    uchar DevBGet();
    uchar DevCGet();

    virtual void Update8255();
    void Update8255_();
    void Reset();

    bool ParAisInput = false;
    bool ParBisInput = false;
    bool ParCHisInput = false;
    bool ParCLisInput = false;
protected:
    int ParAint = 0;
    int ParAext = 0;
    int ParA = 0;
    int ParBint = 0;
    int ParBext = 0;
    int ParB = 0;
    int ParCint = 0;
    int ParCext = 0;
    int ParC = 0;
    int ParAmode = 0;
    int ParBmode = 0;
};

#endif // I8255_H
