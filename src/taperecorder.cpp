#include "taperecorder.h"

TapeRecorder::TapeRecorder()
{
    WorkState = 0;
    TapeRes = 1;
    TapeLen = 0;
    TapePos = 0;

    LastRead = 0;
    LastWrite = 0;

    BufLong = new llong[1];
    BufChar = new uchar[1];

    for (int i = 0; i < OscillatorPeriod; i++)
    {
        OscillatorWave[i] = sin(2 * M_PI * (double)i / (double)OscillatorPeriod);
    }
}

TapeRecorder::~TapeRecorder()
{
    delete[] BufLong;
    delete[] BufChar;
}

void TapeRecorder::SetState(int S)
{
    if (ExecMutex.try_lock())
    {
        if ((WorkState != 5) && (WorkState != 6))
        {

            // Zakonczenie odtwarzania lub nagrywania
            if (((WorkState == 1) || (WorkState == 2)) && (S != 1) && (S != 2))
            {
                while (!TapeAudioBuf.empty())
                {
                    TapeAudioBuf.pop();
                }
            }

            // Rozpoczecie odtwarzania lub nagrywania
            if (((S == 1) || (S == 2)) && (WorkState != 1) && (WorkState != 2))
            {
                while (!TapeAudioBuf.empty())
                {
                    TapeAudioBuf.pop();
                }

                int I = TapeAudioPrebuffer;
                while (I > 0)
                {
                    TapeAudioBuf.push(0);
                    I--;
                }
                TapeAudioCounterTick = 0;
                TapeAudioCounterSmp = 0;
            }



            WorkState = S;
        }
        ExecMutex.unlock();
    }
}

void TapeRecorder::Clock()
{
    switch (WorkState)
    {
        case 1: // Odtwarzanie
            if (TapePos < (TapeLen - 1))
            {
                if ((TapePos % TapeRes) == 0)
                {
                    LastRead = TapeData[TapePos / TapeRes];

                    // Uzyte wartosci stale to:
                    // 48000 - czestotliwosc probkowania
                    // 4000000 - czestotliwosc taktowania procesora
                    // Zera sie skracaja
                    TapeAudioCounterTick = TapeAudioCounterTick + (48 * TapeRes);
                    llong TapeAudioCounterSmp_ = TapeAudioCounterTick / 4000;

                    while (TapeAudioCounterSmp < TapeAudioCounterSmp_)
                    {
                        TapeAudioBuf.push(LastRead);
                        TapeAudioCounterSmp++;
                    }
                }
                TapePos++;
            }
            else
            {
                WorkState = 0;
            }
            break;
        case 2: // Nagrywanie
            if (TapePos < (TapeLen - 1))
            {
                if ((TapePos % TapeRes) == 0)
                {
                    TapeData[TapePos / TapeRes] = LastWrite;

                    // Uzyte wartosci stale to:
                    // 48000 - czestotliwosc probkowania
                    // 4000000 - czestotliwosc taktowania procesora
                    // Zera sie skracaja
                    TapeAudioCounterTick = TapeAudioCounterTick + (48 * TapeRes);
                    llong TapeAudioCounterSmp_ = TapeAudioCounterTick / 4000;

                    while (TapeAudioCounterSmp < TapeAudioCounterSmp_)
                    {
                        TapeAudioBuf.push(LastWrite);
                        TapeAudioCounterSmp++;
                    }
                }
                TapePos++;
            }
            else
            {
                WorkState = 0;
            }
            break;
        case 3: // Przewijanie do przodu
            if (TapePos < (TapeLen - 1))
            {
                TapePos += 10;
                if (TapePos > (TapeLen - 1))
                {
                    TapePos = (TapeLen - 1);
                }
            }
            else
            {
                WorkState = 0;
            }
            break;
        case 4: // Przewijanie do tylu
            if (TapePos > 0)
            {
                TapePos -= 10;
                if (TapePos < 0)
                {
                    TapePos = 0;
                }
            }
            else
            {
                WorkState = 0;
            }
            break;
        //case 5: // Odczyt z pliku
        //    break;
        //case 6: // Zapis do pliku
        //    break;
    }
}

void TapeRecorder::TapeGenerate(llong Len, llong Res)
{
    TapeLen = Len;
    TapeRes = Res;
    TapePos = 0;
    TapeData.resize(0);
    TapeData.reserve(TapeLen);
    TapeData.resize(TapeLen, false);
}

void TapeRecorder::FileLoad(string FN)
{
    ExecMutex.lock();
    WorkState = 5;
    ExecMutex.unlock();
    thread Thr(&TapeRecorder::FileLoadThr, this, FN);
    Thr.detach();
}

void TapeRecorder::FileLoadThr(string FN)
{
    ifstream F(FN.c_str(), ios::in|ios::binary);
    if (F.is_open())
    {
        ExecMutex.lock();

        // Dlugosc tasmy
        F.read((char*)BufLong, 8);
        TapeLen = BufLong[0];

        // Rozdzielczosc tasmy
        F.read((char*)BufLong, 8);
        TapeRes = BufLong[0];

        // Tworzenie struktury tasmy
        TapeData.resize(0);
        TapeData.reserve(TapeLen);
        TapeData.resize(TapeLen, false);

        ExecMutex.unlock();

        // Odczyt danych
        llong I = 0;
        llong II = 0;
        llong DataL = 0;
        llong Step = TapeLen / 100;
        bool DataV = false;
        while (I < TapeLen)
        {
            F.read((char*)BufLong, 8);
            DataL = BufLong[0];

            F.read((char*)BufChar, 1);
            DataV = (BufChar[0] != 0);

            for (II = 0; II < DataL; II++)
            {
                TapeData[I] = DataV;
                if ((I % Step) == 0)
                {
                    ExecMutex.lock();
                    TapePos = I;
                    ExecMutex.unlock();
                }
                I++;
            }
        }

        F.close();
    }
    ExecMutex.lock();
    TapePos = 0;
    WorkState = 0;
    ExecMutex.unlock();
}

void TapeRecorder::FileSave(string FN)
{
    ExecMutex.lock();
    WorkState = 6;
    ExecMutex.unlock();
    thread Thr(&TapeRecorder::FileSaveThr, this, FN);
    Thr.detach();
}

void TapeRecorder::FileSaveThr(string FN)
{
    ofstream F(FN.c_str(), ios::out|ios::binary);
    if (F.is_open())
    {
        ExecMutex.lock();

        // Dlugosc tasmy
        BufLong[0] = TapeLen;
        F.write((char*)BufLong, 8);

        // Rozdzielczosc tasmy
        BufLong[0] = TapeRes;
        F.write((char*)BufLong, 8);

        ExecMutex.unlock();

        // Zapis danych
        bool DataV = false;
        llong DataC = 0;
        llong Step = TapeLen / 100;
        for (llong I = 0; I < TapeLen; I++)
        {
            if (TapeData[I] != DataV)
            {
                if (DataC > 0)
                {
                    BufLong[0] = DataC;
                    F.write((char*)BufLong, 8);
                    if (DataV)
                    {
                        BufChar[0] = 255;
                    }
                    else
                    {
                        BufChar[0] = 0;
                    }
                    F.write((char*)BufChar, 1);
                }
                DataV = TapeData[I];
                DataC = 1;
            }
            else
            {
                DataC++;
            }
            if ((I % Step) == 0)
            {
                ExecMutex.lock();
                TapePos = I;
                ExecMutex.unlock();
            }
        }
        if (DataC > 0)
        {
            BufLong[0] = DataC;
            F.write((char*)BufLong, 8);
            if (DataV)
            {
                BufChar[0] = 255;
            }
            else
            {
                BufChar[0] = 0;
            }
            F.write((char*)BufChar, 1);
        }
        F.close();
    }
    ExecMutex.lock();
    TapePos = 0;
    WorkState = 0;
    ExecMutex.unlock();
}

string TapeRecorder::GetInfo()
{
    if ((WorkState == 5) || (WorkState == 6))
    {
        llong XXX = 0;
        if (ExecMutex.try_lock())
        {
            if (TapeLen > 0)
            {
                XXX = TapePos * 100 / TapeLen;
            }
            ExecMutex.unlock();
        }
        return to_string(XXX) + "%";
    }


    llong Sec = TapeLen / 4000000;
    llong Min = Sec / 60;
    Sec = Sec % 60;
    string InfoL = (Min < 10 ? "0" : "") + to_string(Min) + ":" + (Sec < 10 ? "0" : "") + to_string(Sec);

    Sec = TapePos / 4000000;
    Min = Sec / 60;
    Sec = Sec % 60;
    string InfoP = (Min < 10 ? "0" : "") + to_string(Min) + ":" + (Sec < 10 ? "0" : "") + to_string(Sec);

    string InfoS;
    switch (WorkState)
    {
        case 0: InfoS = ""; break;
        case 1: InfoS = "<"; break;
        case 2: InfoS = "(  )"; break;
        case 3: InfoS = "<<"; break;
        case 4: InfoS = ">>"; break;
    }

    //return InfoP + " / " + InfoL + " (" + to_string(TapeRes) + ")  " + InfoS;
    return InfoP + " / " + InfoL + "  " + InfoS;
}

bool TapeRecorder::TapeRead()
{
    if (WorkState == 1)
    {
        return LastRead;
    }
    else
    {
        return false;
    }
}

void TapeRecorder::TapeWrite(bool Val)
{
    if (WorkState == 2)
    {
        LastWrite = Val;
    }
}

bool TapeRecorder::IsIdle()
{
    return (WorkState == 0);
}

int TapeRecorder::GetSample(double Volume)
{
    OscillatorCounter++;
    if (OscillatorCounter >= OscillatorPeriod)
    {
        OscillatorCounter = 0;
    }

    if (!TapeAudioBuf.empty())
    {
        if (TapeAudioBuf.front())
        {
            TapeAudioBuf.pop();
            return Volume * OscillatorWave[OscillatorCounter];
        }
        else
        {
            TapeAudioBuf.pop();
        }
    }
    return 0;
}
