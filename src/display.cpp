#include "display.h"

Display::Display()
{
    Reset();
}

void Display::Update(uchar Segment, uchar Value)
{
    Value = ~Value;
    int N = -1;
    switch (Segment)
    {
        case 0x00: N = 7; break;
        case 0x20: N = 6; break;
        case 0x40: N = 5; break;
        case 0x60: N = 4; break;
        case 0x80: N = 3; break;
        case 0xA0: N = 2; break;
        case 0xC0: N = 1; break;
        case 0xE0: N = 0; break;
    }
    if (Value & b10000000) { DisplayTimer[N][7] = Refresh; }
    if (Value & b01000000) { DisplayTimer[N][6] = Refresh; }
    if (Value & b00100000) { DisplayTimer[N][5] = Refresh; }
    if (Value & b00010000) { DisplayTimer[N][4] = Refresh; }
    if (Value & b00001000) { DisplayTimer[N][3] = Refresh; }
    if (Value & b00000100) { DisplayTimer[N][2] = Refresh; }
    if (Value & b00000010) { DisplayTimer[N][1] = Refresh; }
    if (Value & b00000001) { DisplayTimer[N][0] = Refresh; }
}

void Display::Clock()
{
    for (int I = 0; I < 8; I++)
    {
        Buf[I] = 0;
        for (int J = 0; J < 8; J++)
        {
            if (DisplayTimer[I][J] > 0)
            {
                Buf[I] |= 1 << J;
                DisplayTimer[I][J]--;
            }
        }
    }
}

void Display::Reset()
{
    for (int I = 0; I < 8; I++)
    {
        Buf0[I] = 0xFF;
        Buf[I] = 0x00;
        for (int J = 0; J < 8; J++)
        {
            DisplayTimer[I][J] = 0;
        }
    }
}

string Display::GetDisplay()
{
    bool NoChange = true;
    if (Buf0[0] != Buf[0]) { NoChange = false; }
    if (Buf0[1] != Buf[1]) { NoChange = false; }
    if (Buf0[2] != Buf[2]) { NoChange = false; }
    if (Buf0[3] != Buf[3]) { NoChange = false; }
    if (Buf0[4] != Buf[4]) { NoChange = false; }
    if (Buf0[5] != Buf[5]) { NoChange = false; }
    if (Buf0[6] != Buf[6]) { NoChange = false; }
    if (Buf0[7] != Buf[7]) { NoChange = false; }
    if (NoChange)
    {
        return "";
    }
    Buf0[0] = Buf[0];
    Buf0[1] = Buf[1];
    Buf0[2] = Buf[2];
    Buf0[3] = Buf[3];
    Buf0[4] = Buf[4];
    Buf0[5] = Buf[5];
    Buf0[6] = Buf[6];
    Buf0[7] = Buf[7];

    stringstream S;
    int I;

    S << endl;
    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00000001) ? "   \u2500\u2500\u2500\u2500   " : "          ");
    }
    S << endl;

    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00100000) ? "  \u2502  " : "     ") << ((Buf[I] & b00000010) ? "  \u2502  " : "     ");
    }
    S << endl;
    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00100000) ? "  \u2502  " : "     ") << ((Buf[I] & b00000010) ? "  \u2502  " : "     ");
    }
    S << endl;

    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b01000000) ? "   \u2500\u2500\u2500\u2500   " : "          ");
    }
    S << endl;

    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00010000) ? "  \u2502  " : "     ") << ((Buf[I] & b00000100) ? "  \u2502  " : "     ");
    }
    S << endl;
    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00010000) ? "  \u2502  " : "     ") << ((Buf[I] & b00000100) ? "  \u2502  " : "     ");
    }
    S << endl;

    for (I = 0; I < 8; I++)
    {
        S << ((Buf[I] & b00001000) ? "   \u2500\u2500\u2500\u2500" : "       ") << ((Buf[I] & b10000000) ? " \u25CF " : "   ");
    }

    return S.str();
}
