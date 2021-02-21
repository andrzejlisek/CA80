#include "i8255_sys.h"

I8255_SYS::I8255_SYS()
{

}

I8255_SYS::~I8255_SYS()
{

}

void I8255_SYS::Update8255()
{
    // Piny 0, 1, 2 portu A sa podlaczone do masy
    ParAext = ParAext & b11111000;

    // Odcyt stanu klawiatury
    uchar KeySet = _Keyboard->GetDevState(ParCint & b00001111);
    ParAext = KeySet;

    // Aktualizacja wyswietlacza
    _Display->Update(ParCint & b11100000, ParBint);

    // Zapis na tasme
    _TapeRecorder->TapeWrite(ParCint & b00010000);

    // Odczyt z tasmy
    if (_TapeRecorder->TapeRead())
    {
        ParAext = ParAext | b10000000;
    }
    else
    {
        ParAext = ParAext & b01111111;
    }

    Update8255_();
}
