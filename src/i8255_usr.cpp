#include "i8255_usr.h"

I8255_USR::I8255_USR()
{

}

I8255_USR::~I8255_USR()
{

}

void I8255_USR::Update8255()
{
    Update8255_();

    if (ParC & b00000001)
    {
        _Music->IsActive = 0;
    }
    else
    {
        _Music->IsActive = 1;
    }
    _Music->SetPitch(ParB);
}
