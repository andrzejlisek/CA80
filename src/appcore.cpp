#include "appcore.h"

AppCore::AppCore()
{
    _CpuMem = new CpuMem();
    _Display = new Display();
    _I8255_SYS = new I8255_SYS();
    _I8255_USR = new I8255_USR();
    _Keyboard = new Keyboard();
    _Sound = new Sound();
    _TapeRecorder = new TapeRecorder();
    _ExtDateTime = new ExtDateTime();
    _Music = new Music();

    _I8255_SYS->_Keyboard = _Keyboard;
    _I8255_SYS->_Display = _Display;
    _I8255_USR->_Music = _Music;
    _CpuMem->_Display = _Display;
    _CpuMem->_I8255_SYS = _I8255_SYS;
    _CpuMem->_I8255_USR = _I8255_USR;
    _CpuMem->_Sound = _Sound;
    _CpuMem->_ExtDateTime = _ExtDateTime;

    _CpuMem->_TapeRecorder = _TapeRecorder;
    _I8255_SYS->_TapeRecorder = _TapeRecorder;

    string AppDir = Eden::ApplicationDirectory();
    string KeyFile = AppDir + "Keys.txt";
    string ConfFile = AppDir + "Config.txt";

    // Ladowanie konfiguracji z pliku
    fstream F(ConfFile.c_str(), ios::in);
    string Buf;

    if (F.is_open())
    {
        // Pliki ROM
        getline(F, Buf);
        int L = stoi(Buf);
        for (int I = 0; I < L; I++)
        {
            getline(F, Buf);
            _CpuMem->RomBegin.push_back(Eden::HexToInt(Buf));
            _CpuMem->RomEnd.push_back(-1);
            getline(F, Buf);
            _CpuMem->RomFile.push_back(Buf);
        }

        // Zapisywalnosc pamieci
        getline(F, Buf);
        _CpuMem->Mem0Writable = (Buf == "1");
        getline(F, Buf);
        _CpuMem->Mem1Writable = (Buf == "1");
        getline(F, Buf);
        _CpuMem->Mem2Writable = (Buf == "1");
        getline(F, Buf);
        _CpuMem->Mem3Writable = (Buf == "1");

        // Wielkosci pamieci
        getline(F, Buf);
        _CpuMem->Mem0Size = Eden::HexToInt(Buf);
        getline(F, Buf);
        _CpuMem->Mem1Size = Eden::HexToInt(Buf);
        getline(F, Buf);
        _CpuMem->Mem2Size = Eden::HexToInt(Buf);
        getline(F, Buf);
        _CpuMem->Mem3Size = Eden::HexToInt(Buf);

        // Inne ustawienia
        getline(F, Buf);
        _Keyboard->NewCA80 = (Buf == "1");
        getline(F, Buf);
        SoundAudible = (Buf == "1");
        getline(F, Buf);
        _CpuMem->NMIExists = (Buf == "1");
        getline(F, Buf);
        _CpuMem->INTExists = (Buf == "1");
        getline(F, Buf);
        _CpuMem->NMITime = Eden::ToLong(Buf);
        getline(F, Buf);
        _CpuMem->INTTime = Eden::ToLong(Buf);
        getline(F, Buf);
        TimerPeriod = Eden::ToInt(Buf);

        // Ustawienia dzwieku
        getline(F, Buf);
        SoundBufSize = Eden::ToInt(Buf);
        getline(F, Buf);
        SoundChunkSize = Eden::ToInt(Buf);
        getline(F, Buf);
        SoundVolume = Eden::ToInt(Buf);
        getline(F, Buf);
        MusicVolume = Eden::ToInt(Buf);
        getline(F, Buf);
        _Sound->TriggerTime = Eden::ToLong(Buf);


        F.close();
    }

    _CpuMem->SortRoms();

    _CpuMem->LoadRom(true);


    // Definiowanie powiazan klawiszy na klawiaturze z klawiszami komputera
    // Zapisywanie definicji klawiszy
    fstream F0(KeyFile.c_str(), ios::in);
    if (F0.is_open())
    {
        getline(F0, Buf);
        int L = stoi(Buf);
        for (int I = 0; I < L; I++)
        {
            getline(F0, Buf);
            int I0 = stoi(Buf);
            getline(F0, Buf);
            KeyMap[I0] = stoi(Buf);
        }
        F0.close();
    }


    // Wizualizacja klawiszy
    VisKey[0x00] = "0";
    VisKey[0x01] = "1";
    VisKey[0x02] = "2";
    VisKey[0x03] = "3";
    VisKey[0x04] = "4";
    VisKey[0x05] = "5";
    VisKey[0x06] = "6";
    VisKey[0x07] = "7";
    VisKey[0x08] = "8";
    VisKey[0x09] = "9";
    VisKey[0x0A] = "A";
    VisKey[0x0B] = "B";
    VisKey[0x0C] = "C";
    VisKey[0x0D] = "D";
    VisKey[0x0E] = "E";
    VisKey[0x0F] = "F";
    VisKey[0x10] = "G";
    VisKey[0x11] = ".";
    VisKey[0x12] = "=";
    VisKey[0x13] = "M";
    VisKey[0x14] = "W";
    VisKey[0x15] = "X";
    VisKey[0x16] = "Y";
    VisKey[0x17] = "Z";

    /*
    KeyMap[48] = 0x00;
    KeyMap[49] = 0x01;
    KeyMap[50] = 0x02;
    KeyMap[51] = 0x03;
    KeyMap[52] = 0x04;
    KeyMap[53] = 0x05;
    KeyMap[54] = 0x06;
    KeyMap[55] = 0x07;
    KeyMap[56] = 0x08;
    KeyMap[57] = 0x09;
    KeyMap[65] = 0x0A;
    KeyMap[66] = 0x0B;
    KeyMap[67] = 0x0C;
    KeyMap[68] = 0x0D;
    KeyMap[69] = 0x0E;
    KeyMap[70] = 0x0F;
    KeyMap[71] = 0x10;
    KeyMap[46] = 0x11;
    KeyMap[61] = 0x12;
    KeyMap[77] = 0x13;
    KeyMap[87] = 0x14;
    KeyMap[88] = 0x15;
    KeyMap[89] = 0x16;
    KeyMap[90] = 0x17;
    */

    Reset();
}

AppCore::~AppCore()
{
    string AppDir = Eden::ApplicationDirectory();
    string KeyFile = AppDir + "Keys.txt";
    string ConfFile = AppDir + "Config.txt";

    string Buf;

    // Zapisanie biezacej konfiguracji do pliku
    fstream F(ConfFile.c_str(), ios::out);
    if (F.is_open())
    {
        // Pliki ROM
        Buf = to_string(_CpuMem->RomFile.size());
        F << Buf << endl;
        for (uint I = 0; I < _CpuMem->RomFile.size(); I++)
        {
            F << Eden::IntToHex16(_CpuMem->RomBegin[I]) << endl;
            F << _CpuMem->RomFile[I] << endl;
        }

        // Zapisywalnosc pamieci
        F << (_CpuMem->Mem0Writable ? "1" : "0") << endl;
        F << (_CpuMem->Mem1Writable ? "1" : "0") << endl;
        F << (_CpuMem->Mem2Writable ? "1" : "0") << endl;
        F << (_CpuMem->Mem3Writable ? "1" : "0") << endl;

        // Wielkosci pamieci
        F << Eden::IntToHex16(_CpuMem->Mem0Size) << endl;
        F << Eden::IntToHex16(_CpuMem->Mem1Size) << endl;
        F << Eden::IntToHex16(_CpuMem->Mem2Size) << endl;
        F << Eden::IntToHex16(_CpuMem->Mem3Size) << endl;

        // Inne ustawienia
        F << (_Keyboard->NewCA80 ? "1" : "0") << endl;
        F << (SoundAudible ? "1" : "0") << endl;
        F << (_CpuMem->NMIExists ? "1" : "0") << endl;
        F << (_CpuMem->INTExists ? "1" : "0") << endl;
        F << _CpuMem->NMITime << endl;
        F << _CpuMem->INTTime << endl;
        F << TimerPeriod << endl;

        // Ustawienia dzwieku
        F << SoundBufSize << endl;
        F << SoundChunkSize << endl;
        F << SoundVolume << endl;
        F << MusicVolume << endl;
        F << _Sound->TriggerTime << endl;

        F.close();
    }

    // Zapisywanie definicji klawiszy
    fstream F0(KeyFile.c_str(), ios::out);
    if (F0.is_open())
    {
        F0 << to_string(KeyMap.size()) << endl;
        map<int, int>::iterator MIt;
        for (MIt = KeyMap.begin(); MIt != KeyMap.end(); MIt++)
        {
            F0 << to_string(MIt->first) << endl;
            F0 << to_string(MIt->second) << endl;
        }
        F0.close();
    }

    delete _CpuMem;
    delete _Display;
    delete _Keyboard;
    delete _I8255_SYS;
    delete _I8255_USR;
    delete _Sound;
    delete _TapeRecorder;
    delete _ExtDateTime;
    delete _Music;
}

void AppCore::Reset()
{
    _CpuMem->Reset();
    _I8255_SYS->Reset();
    _I8255_USR->Reset();
    _Sound->Reset();
    _Display->Reset();
}
