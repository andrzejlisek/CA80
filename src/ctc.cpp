#include "ctc.h"

CTC::CTC()
{
    Reset();
}

///
/// \brief CTC::Reset - Resetowanie CTC
///
void CTC::Reset()
{
    for (int I = 0; I < 4; I++)
    {
        CCR_7[I] = 0;
        CCR_6[I] = 0;
        CCR_5[I] = 0;
        CCR_4[I] = 0;
        CCR_3[I] = 0;
        CCR_2[I] = 0;
        CCR_1[I] = 0;
        DownCounter[I] = 0;
        TCR[I] = 0;
        PrescalerCounter[I] = 0;
        PrescalerPeriod[I] = 0;
        InterruptFlag[I] = 0;
    }
}


///
/// \brief CTC::Set - Ustawianie wartosci na wskazanym kanale
/// \param ChN
/// \param Val
///
void CTC::Set(uchar ChN, uchar Val)
{
    if (CCR_2[ChN])
    {
        CCR_2[ChN] = 0;
        TCR[ChN] = Val;
        DownCounter[ChN] = Val;
        PrescalerCounter[ChN] = 0;
        if (CCR_6[ChN])
        {
            PrescalerPeriod[ChN] = 0;
        }
        else
        {
            if (CCR_5[ChN])
            {
                PrescalerPeriod[ChN] = 256;
            }
            else
            {
                PrescalerPeriod[ChN] = 16;
            }
        }
    }
    else
    {
        if (Val & b00000001)
        {
            //= 0 - zablokowanie generacji przerwania
            //= 1 - odblokowanie generacji przerwania
            CCR_7[ChN] = Val & b10000000;

            //= 0 - tryb czasowy
            //= 1 - tryb licznikowy
            CCR_6[ChN] = Val & b01000000;

            //tylko w trybie czasowym
            //= 0 - podzielnik ustawiony na 16
            //= 1 - podzielnik ustawiony na 256
            CCR_5[ChN] = Val & b00100000;

            //tylko w trybie czasowym
            //= 0 - opadające zbocze CLK/TRG inicjuje zliczanie
            //= 1 - jak wyżej lecz zbocze narastające
            CCR_4[ChN] = Val & b00010000;

            //tylko w trybie czasowym
            //= 0 - gdy (b00000100)=0 układ w trybie czasowym rozpoczyna pracę od następnego cyklu maszynowego
            //= 1 - gdy (b00000100)=0 układ rozpoczyna działanie po odpowiedniej zmianie na CLK/TRG ( lub )
            //= 0 - gdy (b00000100)=1 jak wyżej lecz po załadowaniu stałej czasowej
            CCR_3[ChN] = Val & b00001000;

            //= 0 - nie będzie słowa z wartością początkową licznika
            //= 1 - następnym słowem będzie wartość początkowa licznika
            CCR_2[ChN] = Val & b00000100;

            //= 0 - praca ciągła
            //= 1 - zatrzymanie do czasu załadowania nowej stałej czasowej
            CCR_1[ChN] = Val & b00000010;
            if (CCR_1[ChN])
            {
                PrescalerPeriod[ChN] = 0;
            }
        }
        else
        {
            InterruptAddr_[ChN] = Val & b11111000;
            if ((ChN == 1) || (ChN == 3))
            {
                InterruptAddr_[ChN] += b00000010;
            }
            if ((ChN == 2))
            {
                InterruptAddr_[ChN] += b00000100;
            }
        }
    }
}

///
/// \brief CTC::Get - Pobieranie wartosci na wskazanym kanale
/// \param ChN
/// \return
///
uchar CTC::Get(uchar ChN)
{
    uchar Val = 0;
    if (CCR_7[ChN]) Val += 128;
    if (CCR_6[ChN]) Val += 64;
    if (CCR_5[ChN]) Val += 32;
    if (CCR_4[ChN]) Val += 16;
    if (CCR_3[ChN]) Val += 8;
    if (CCR_2[ChN]) Val += 4;
    if (CCR_1[ChN]) Val += 2;
    return Val;
}

///
/// \brief CTC::Trigger - Pobudzenie licznika poprzez zdarzenie zewnetrzne
/// \param ChN
///
void CTC::Trigger(uchar ChN)
{
    DownCounter[ChN]--;
    if (DownCounter[ChN] == 0)
    {
        DownCounter[ChN] = TCR[ChN];
        if (CCR_7[ChN])
        {
            bool Allowed = true;
            for (int II = 0; II <= ChN; II++)
            {
                if (InterruptFlag[II] != 0)
                {
                    Allowed = false;
                }
            }
            if (Allowed)
            {
                InterruptFlag[ChN] = 1;
            }
        }
    }
}

///
/// \brief CTC::Clock - Taktowanie zegara
///
void CTC::Clock()
{
    if (CTC0Exists)
    {
        bool Trg[4];
        Trg[0] = true;
        Trg[1] = true;
        Trg[2] = true;
        Trg[3] = true;
        CTC0TimeC++;
        if (CTC0TimeC >= CTC0Time)
        {
            CTC0TimeC = 0;
            if (CTC0Time > 0)
            {
                Trigger(0);
            }
            Trg[0] = false;
        }

        for (int I = 0; I < 4; I++)
        {
            if (PrescalerPeriod[I])
            {
                PrescalerCounter[I]++;
                if (PrescalerCounter[I] >= PrescalerPeriod[I])
                {
                    PrescalerCounter[I] = 0;
                    if (Trg[I])
                    {
                        Trigger(I);
                    }
                }
            }
        }
    }
}

///
/// \brief CTC::Interrupt - Sprawdzanie, czy nalezy wyzwolic przerwanie
///
bool CTC::Interrupt()
{
    if (InterruptFlag[0])
    {
        if (InterruptFlag[0] == 1)
        {
            InterruptFlag[0] = 2;
            return true;
        }
        return false;
    }
    if (InterruptFlag[1])
    {
        if (InterruptFlag[1] == 1)
        {
            InterruptFlag[1] = 2;
            return true;
        }
        return false;
    }
    if (InterruptFlag[2])
    {
        if (InterruptFlag[2] == 1)
        {
            InterruptFlag[2] = 2;
            return true;
        }
        return false;
    }
    if (InterruptFlag[3])
    {
        if (InterruptFlag[3] == 1)
        {
            InterruptFlag[3] = 2;
            return true;
        }
        return false;
    }
    return false;
}

///
/// \brief CTC::InterruptAddr - Pobieranie adresu obslugi przerwania
/// \return
///
uchar CTC::InterruptAddr()
{
    if (InterruptFlag[0] == 2) { return InterruptAddr_[0]; }
    if (InterruptFlag[1] == 2) { return InterruptAddr_[1]; }
    if (InterruptFlag[2] == 2) { return InterruptAddr_[2]; }
    if (InterruptFlag[3] == 2) { return InterruptAddr_[3]; }
    return 0;
}

///
/// \brief CTC::InterruptReset - Resetowanie stanu przerwania po jego obsluzeniu
///
void CTC::InterruptReset()
{
    if (InterruptFlag[0] == 2) { InterruptFlag[0] = 0; return; }
    if (InterruptFlag[1] == 2) { InterruptFlag[1] = 0; return; }
    if (InterruptFlag[2] == 2) { InterruptFlag[2] = 0; return; }
    if (InterruptFlag[3] == 2) { InterruptFlag[3] = 0; return; }
}
