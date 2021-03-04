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
    _CTC = new CTC();

    _I8255_SYS->_Keyboard = _Keyboard;
    _I8255_SYS->_Display = _Display;
    _I8255_USR->_Music = _Music;
    _CpuMem->_Display = _Display;
    _CpuMem->_I8255_SYS = _I8255_SYS;
    _CpuMem->_I8255_USR = _I8255_USR;
    _CpuMem->_Sound = _Sound;
    _CpuMem->_ExtDateTime = _ExtDateTime;
    _CpuMem->_CTC = _CTC;

    _CpuMem->_TapeRecorder = _TapeRecorder;
    _I8255_SYS->_TapeRecorder = _TapeRecorder;

    // Ladowanie konfiguracji z pliku
    EdenClass::ConfigFile CF;
    CF.FileLoad(Eden::ApplicationDirectory() + "Config.txt");

    // Pliki ROM
    int L = CF.ParamGetI("RomCount");
    for (int I = 0; I < L; I++)
    {
        _CpuMem->RomBegin.push_back(CF.ParamGetI("Rom" + to_string(I) + "Addr"));
        _CpuMem->RomEnd.push_back(-1);
        _CpuMem->RomFile.push_back(CF.ParamGetS("Rom" + to_string(I) + "File"));
    }

    // Zapisywalnosc pamieci
    _CpuMem->Mem0Writable = CF.ParamGetB("Mem0Writable");
    _CpuMem->Mem1Writable = CF.ParamGetB("Mem1Writable");
    _CpuMem->Mem2Writable = CF.ParamGetB("Mem2Writable");
    _CpuMem->Mem3Writable = CF.ParamGetB("Mem3Writable");

    // Wielkosci pamieci
    _CpuMem->Mem0Size = CF.ParamGetI("Mem0Size");
    _CpuMem->Mem1Size = CF.ParamGetI("Mem1Size");
    _CpuMem->Mem2Size = CF.ParamGetI("Mem2Size");
    _CpuMem->Mem3Size = CF.ParamGetI("Mem3Size");

    // Inne ustawienia
    _Keyboard->NewCA80 = CF.ParamGetB("Generation");
    SoundAudible = CF.ParamGetB("SoundAudible");
    _CpuMem->NMIExists = CF.ParamGetB("NMI");
    _CpuMem->INTExists = CF.ParamGetB("INT");
    _CpuMem->NMITime = CF.ParamGetL("NMIPeriod");
    _CpuMem->INTTime = CF.ParamGetL("INTPeriod");
    _CTC->CTC0Exists = CF.ParamGetB("CTC0");
    _CTC->CTC0Time = CF.ParamGetL("CTC0Period");
    TimerPeriod = CF.ParamGetI("TimerPeriod");

    // Ustawienia dzwieku
    SoundBufSize = CF.ParamGetI("SoundBufSize");
    SoundChunkSize = CF.ParamGetI("SoundChunkSize");
    SoundMinFill = CF.ParamGetI("SoundMinFill");
    SoundVolume = CF.ParamGetI("SoundVolume");
    MusicVolume = CF.ParamGetI("MusicVolume");
    TapeVolume = CF.ParamGetI("TapeVolume");
    _TapeRecorder->TapeAudioPrebuffer = CF.ParamGetI("TapePrebuffer");
    _Sound->TriggerTime = CF.ParamGetL("TriggerTime");

    _CpuMem->SortRoms();

    _CpuMem->LoadRom(true);


    // Definiowanie powiazan klawiszy na klawiaturze z klawiszami komputera
    // Zapisywanie definicji klawiszy
    L = CF.ParamGetI("KeyCount");
    for (int I = 0; I < L; I++)
    {
        int I0 = CF.ParamGetI("Key" + to_string(I) + "Code");
        KeyMap[I0] = CF.ParamGetI("Key" + to_string(I) + "Function");
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

    Reset();
}

AppCore::~AppCore()
{
    EdenClass::ConfigFile CF;

    // Zapisanie biezacej konfiguracji do pliku
    CF.ParamClear();

    // Pliki ROM
    CF.ParamSet("RomCount", (llong)(_CpuMem->RomFile.size()));
    for (uint I = 0; I < _CpuMem->RomFile.size(); I++)
    {
        CF.ParamSet("Rom" + to_string(I) + "Addr", _CpuMem->RomBegin[I]);
        CF.ParamSet("Rom" + to_string(I) + "File", _CpuMem->RomFile[I]);
    }

    // Zapisywalnosc pamieci
    CF.ParamSet("Mem0Writable", _CpuMem->Mem0Writable);
    CF.ParamSet("Mem1Writable", _CpuMem->Mem1Writable);
    CF.ParamSet("Mem2Writable", _CpuMem->Mem2Writable);
    CF.ParamSet("Mem3Writable", _CpuMem->Mem3Writable);

    // Wielkosci pamieci
    CF.ParamSet("Mem0Size", _CpuMem->Mem0Size);
    CF.ParamSet("Mem1Size", _CpuMem->Mem1Size);
    CF.ParamSet("Mem2Size", _CpuMem->Mem2Size);
    CF.ParamSet("Mem3Size", _CpuMem->Mem3Size);

    // Inne ustawienia
    CF.ParamSet("Generation", _Keyboard->NewCA80);
    CF.ParamSet("SoundAudible", SoundAudible);
    CF.ParamSet("NMI", _CpuMem->NMIExists);
    CF.ParamSet("INT", _CpuMem->INTExists);
    CF.ParamSet("NMIPeriod", _CpuMem->NMITime);
    CF.ParamSet("INTPeriod", _CpuMem->INTTime);
    CF.ParamSet("CTC0", _CTC->CTC0Exists);
    CF.ParamSet("CTC0Period", _CTC->CTC0Time);
    CF.ParamSet("TimerPeriod", TimerPeriod);

    // Ustawienia dzwieku
    CF.ParamSet("SoundBufSize", SoundBufSize);
    CF.ParamSet("SoundChunkSize", SoundChunkSize);
    CF.ParamSet("SoundMinFill", SoundMinFill);
    CF.ParamSet("SoundVolume", SoundVolume);
    CF.ParamSet("MusicVolume", MusicVolume);
    CF.ParamSet("TapeVolume", TapeVolume);
    CF.ParamSet("TapePrebuffer", _TapeRecorder->TapeAudioPrebuffer);
    CF.ParamSet("TriggerTime", (llong)(_Sound->TriggerTime));

    // Zapisywanie definicji klawiszy
    CF.ParamSet("KeyCount", (llong)(KeyMap.size()));
    map<int, int>::iterator MIt;
    int I = 0;
    for (MIt = KeyMap.begin(); MIt != KeyMap.end(); MIt++)
    {
        CF.ParamSet("Key" + to_string(I) + "Code", (MIt->first));
        CF.ParamSet("Key" + to_string(I) + "Function", (MIt->second));
        I++;
    }

    CF.FileSave(Eden::ApplicationDirectory() + "Config.txt");

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
