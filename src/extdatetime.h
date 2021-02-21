#ifndef EXTDATETIME_H
#define EXTDATETIME_H

#include "eden.h"
#include <iostream>
#include <QDate>
#include <QTime>

class ExtDateTime
{
public:
    ExtDateTime();
    ~ExtDateTime();
    void Set(uchar X);
    uchar Get();
private:
    char DateTimePart;
};

#endif // EXTDATETIME_H
