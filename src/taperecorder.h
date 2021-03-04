#ifndef TAPERECORDER_H
#define TAPERECORDER_H

#include "eden.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

class TapeRecorder
{
public:
    TapeRecorder();
    ~TapeRecorder();
    void FileLoad(string FN);
    void FileSave(string FN);
    void TapeGenerate(llong Len, llong Res);

    void FileLoadThr(string FN);
    void FileSaveThr(string FN);

    void Clock();
    string GetInfo();
    void SetState(int S);
    void TapeWrite(bool Val);
    bool TapeRead();
    bool IsIdle();
    int GetSample(double Volume);
    int TapeAudioPrebuffer = 12000;
private:
    mutex ExecMutex;
    vector<bool> TapeData;
    queue<char> TapeAudioBuf;
    llong TapeAudioCounterSmp;
    llong TapeAudioCounterTick;
    llong TapeAudioProportion;

    llong TapeRes;
    llong TapeLen;
    llong TapePos;
    int WorkState;
    bool LastRead;
    bool LastWrite;

    llong * BufLong;
    uchar * BufChar;

    double OscillatorWave[14];
    int OscillatorPeriod = 14;
    int OscillatorCounter = 0;
};

#endif // TAPERECORDER_H
