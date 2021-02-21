#ifndef I8255_USR_H
#define I8255_USR_H

#include "i8255.h"
#include "music.h"

class I8255_USR : public I8255
{
public:
    I8255_USR();
    ~I8255_USR();
    void Update8255();

    Music * _Music;
};

#endif // I8255_USR_H
