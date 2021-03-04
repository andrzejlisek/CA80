#include "i8255.h"

I8255::I8255()
{
    Reset();
}

I8255::~I8255()
{

}

void I8255::Update8255()
{

}

void I8255::PortASet(uchar Val)
{
    ParAint = Val;
    Update8255();
}

void I8255::PortBSet(uchar Val)
{
    ParBint = Val;
    Update8255();
}

void I8255::PortCSet(uchar Val)
{
    ParCint = Val;
    Update8255();
}

void I8255::PortSSet(uchar Val)
{
    bool B7 = Val & b10000000;
    bool B6 = Val & b01000000;
    bool B5 = Val & b00100000;
    bool B4 = Val & b00010000;
    bool B3 = Val & b00001000;
    bool B2 = Val & b00000100;
    bool B1 = Val & b00000010;
    bool B0 = Val & b00000001;
    if (B7)
    {
        // Sterowanie
        if (B6)
        {
            ParAmode = 2;
        }
        else
        {
            if (B5)
            {
                ParAmode = 1;
            }
            else
            {
                ParAmode = 0;
            }
        }
        ParAisInput = B4;
        ParCHisInput = B3;
        if (B2)
        {
            ParBmode = 1;
        }
        else
        {
            ParBmode = 0;
        }
        ParBisInput = B1;
        ParCLisInput = B0;
    }
    else
    {
        // Ustawianie linii portu C
        int DataC = ParC;

        bool BC7 = DataC & b10000000;
        bool BC6 = DataC & b01000000;
        bool BC5 = DataC & b00100000;
        bool BC4 = DataC & b00010000;
        bool BC3 = DataC & b00001000;
        bool BC2 = DataC & b00000100;
        bool BC1 = DataC & b00000010;
        bool BC0 = DataC & b00000001;

        if (B3)
        {
            if (B2)
            {
                if (B1)
                {
                    // 111 = 7
                    BC7 = B0;
                }
                else
                {
                    // 110 = 6
                    BC6 = B0;
                }
            }
            else
            {
                if (B1)
                {
                    // 101 = 5
                    BC5 = B0;
                }
                else
                {
                    // 100 = 4
                    BC4 = B0;
                }
            }
        }
        else
        {
            if (B2)
            {
                if (B1)
                {
                    // 011 = 3
                    BC3 = B0;
                }
                else
                {
                    // 010 = 2
                    BC2 = B0;
                }
            }
            else
            {
                if (B1)
                {
                    // 001 = 1
                    BC1 = B0;
                }
                else
                {
                    // 000 = 0
                    BC0 = B0;
                }
            }
        }

        ParCint = 0;
        if (BC7) { ParCint = ParCint | b10000000; }
        if (BC6) { ParCint = ParCint | b01000000; }
        if (BC5) { ParCint = ParCint | b00100000; }
        if (BC4) { ParCint = ParCint | b00010000; }
        if (BC3) { ParCint = ParCint | b00001000; }
        if (BC2) { ParCint = ParCint | b00000100; }
        if (BC1) { ParCint = ParCint | b00000010; }
        if (BC0) { ParCint = ParCint | b00000001; }
    }
    Update8255();
}

uchar I8255::PortAGet()
{
    Update8255();
    return ParA;
}

uchar I8255::PortBGet()
{
    Update8255();
    return ParB;
}

uchar I8255::PortCGet()
{
    Update8255();
    return ParC;
}


void I8255::Update8255_()
{
    if (ParAisInput)
    {
        ParA = ParAext;
    }
    else
    {
        ParA = ParAint;
    }
    if (ParBisInput)
    {
        ParB = ParBext;
    }
    else
    {
        ParB = ParBint;
    }
    if ((ParAmode == 0) && (ParBmode == 0))
    {
        int ParCH = (ParCHisInput ? ParCext : ParCint) & b11110000;
        int ParCL = (ParCLisInput ? ParCext : ParCint) & b00001111;
        ParC = ParCH + ParCL;
    }
}

void I8255::Reset()
{
    ParAint = 0;
    ParAext = 0;
    ParA = 0;
    ParBint = 0;
    ParBext = 0;
    ParB = 0;
    ParCint = 0;
    ParCext = 0;
    ParC = 0;

    ParAisInput = false;
    ParBisInput = false;
    ParCHisInput = false;
    ParCLisInput = false;
    ParAmode = 0;
    ParBmode = 0;
}


uchar I8255::DevAGet()
{
    return ParA;
}

uchar I8255::DevBGet()
{
    return ParB;
}

uchar I8255::DevCGet()
{
    return ParC;
}

void I8255::DevASet(uchar Val)
{
    ParAext = Val;
    Update8255();
}

void I8255::DevBSet(uchar Val)
{
    ParBext = Val;
    Update8255();
}

void I8255::DevCSet(uchar Val)
{
    ParCext = Val;
    Update8255();
}

void I8255::DevCHSet(uchar Val)
{
    ParCext = (ParCext & b00001111) | (Val & b11110000);
    Update8255();
}

void I8255::DevCLSet(uchar Val)
{
    ParCext = (ParCext & b11110000) | (Val & b00001111);
    Update8255();
}
