#ifndef TAPERECORDER_H
#define TAPERECORDER_H

#include "eden.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

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
private:
    mutex ExecMutex;
    vector<bool> TapeData;
    llong TapeRes;
    llong TapeLen;
    llong TapePos;
    int WorkState;
    bool LastRead;
    bool LastWrite;

    llong * BufLong;
    uchar * BufChar;
};

#endif // TAPERECORDER_H
