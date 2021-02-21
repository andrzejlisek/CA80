#include "extdatetime.h"

ExtDateTime::ExtDateTime()
{
    DateTimePart = 0;
}

ExtDateTime::~ExtDateTime()
{

}

void ExtDateTime::Set(uchar X)
{
    DateTimePart = X;
}

uchar ExtDateTime::Get()
{
    uchar X = 0;
    switch (DateTimePart)
    {
        // Dzien tygodnia (0 - niedziela, 1 - poniedzialek itd.)
        case 0:
        {
            X = QDate::currentDate().dayOfWeek();
            if (X == 7)
            {
                X = 0;
            }
            break;
        }
        // Dzien miesiaca
        case 1:
        {
            X = QDate::currentDate().day();
            break;
        }
        // Miesiac
        case 2:
        {
            X = QDate::currentDate().month();
            break;
        }
        // Rok (dwie ostatnie cyfry)
        case 3:
        {
            X = (QDate::currentDate().year()) % 100;
            break;
        }
        // Stulecie (dwie pierwsze cyfry roku)
        case 4:
        {
            X = (QDate::currentDate().year()) / 100;
            break;
        }
        // Godzina
        case 5:
        {
            X = QTime::currentTime().hour();
            break;
        }
        // Minuty
        case 6:
        {
            X = QTime::currentTime().minute();
            break;
        }
        // Sekundy
        case 7:
        {
            X = QTime::currentTime().second();
            break;
        }
        // Setne sekund
        case 8:
        {
            X = QTime::currentTime().msec() / 10;
            break;
        }
    }

    // Przeksztalcanie wartosci na BCD
    uchar X0 = X / 10;
    X = X % 10;
    X += (X0 << 4);

    //cout << Eden::IntToHex8(DateTimePart) << "=" << Eden::IntToHex8(X) << endl;

    return X;
}
