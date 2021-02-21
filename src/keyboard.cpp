#include "keyboard.h"

Keyboard::Keyboard()
{
    for (int I = 0; I < 24; I++)
    {
        KeyPressed[I] = false;
    }
}

void Keyboard::KeyPress(int N)
{
    //cout << "PRESS " << N << endl;
    KeyPressed[N] = true;
}

void Keyboard::KeyRelease(int N)
{
    //cout << "RELEASE " << N << endl;
    KeyPressed[N] = false;
}

uchar Keyboard::GetDevState(uchar PortValue)
{
    if (NewCA80)
    {
        uchar Result = b01111110;
        switch (PortValue & b00001111)
        {
        case b00001110: // Z C D E F M
            if (KeyPressed[0x17]) { Result &= b01101110; }
            if (KeyPressed[0x0C]) { Result &= b01110110; }
            if (KeyPressed[0x0D]) { Result &= b01011110; }
            if (KeyPressed[0x0E]) { Result &= b01111010; }
            if (KeyPressed[0x0F]) { Result &= b00111110; }
            if (KeyPressed[0x13]) { Result &= b01111100; }
            break;
        case b00001101: // Y 8 9 A B G
            if (KeyPressed[0x16]) { Result &= b01101110; }
            if (KeyPressed[0x08]) { Result &= b01110110; }
            if (KeyPressed[0x09]) { Result &= b01011110; }
            if (KeyPressed[0x0A]) { Result &= b01111010; }
            if (KeyPressed[0x0B]) { Result &= b00111110; }
            if (KeyPressed[0x10]) { Result &= b01111100; }
            break;
        case b00001011: // X 4 5 6 7 *
            if (KeyPressed[0x15]) { Result &= b01101110; }
            if (KeyPressed[0x04]) { Result &= b01110110; }
            if (KeyPressed[0x05]) { Result &= b01011110; }
            if (KeyPressed[0x06]) { Result &= b01111010; }
            if (KeyPressed[0x07]) { Result &= b00111110; }
            if (KeyPressed[0x11]) { Result &= b01111100; }
            break;
        case b00000111: // W 0 1 2 3 =
            if (KeyPressed[0x14]) { Result &= b01101110; }
            if (KeyPressed[0x00]) { Result &= b01110110; }
            if (KeyPressed[0x01]) { Result &= b01011110; }
            if (KeyPressed[0x02]) { Result &= b01111010; }
            if (KeyPressed[0x03]) { Result &= b00111110; }
            if (KeyPressed[0x12]) { Result &= b01111100; }
            break;
        }
        return Result;
    }
    else
    {
        uchar Result = b01110000;
        switch (PortValue)
        {
        case 0x00: // 2 3 =
            if (KeyPressed[0x02]) { Result &= b01100000; }
            if (KeyPressed[0x03]) { Result &= b01010000; }
            if (KeyPressed[0x12]) { Result &= b00110000; }
            break;
        case 0x01: // X * 1
            if (KeyPressed[0x15]) { Result &= b01100000; }
            if (KeyPressed[0x11]) { Result &= b01010000; }
            if (KeyPressed[0x01]) { Result &= b00110000; }
            break;
        case 0x02: // 4 7 0
            if (KeyPressed[0x04]) { Result &= b01100000; }
            if (KeyPressed[0x07]) { Result &= b01010000; }
            if (KeyPressed[0x00]) { Result &= b00110000; }
            break;
        case 0x03: // 5 6 W
            if (KeyPressed[0x05]) { Result &= b01100000; }
            if (KeyPressed[0x06]) { Result &= b01010000; }
            if (KeyPressed[0x14]) { Result &= b00110000; }
            break;
        case 0x04: // Y G
            if (KeyPressed[0x16]) { Result &= b01100000; }
            if (KeyPressed[0x10]) { Result &= b01010000; }
            break;
        case 0x05: // 9 A
            if (KeyPressed[0x09]) { Result &= b01100000; }
            if (KeyPressed[0x0A]) { Result &= b01010000; }
            break;
        case 0x06: // C F
            if (KeyPressed[0x0C]) { Result &= b01100000; }
            if (KeyPressed[0x0F]) { Result &= b01010000; }
            break;
        case 0x07: // D E
            if (KeyPressed[0x0D]) { Result &= b01100000; }
            if (KeyPressed[0x0E]) { Result &= b01010000; }
            break;
        case 0x08: // Z M
            if (KeyPressed[0x17]) { Result &= b01100000; }
            if (KeyPressed[0x13]) { Result &= b01010000; }
            break;
        case 0x09: // 8 B
            if (KeyPressed[0x08]) { Result &= b01100000; }
            if (KeyPressed[0x0B]) { Result &= b01010000; }
            break;
        }
        return Result;
    }
}
