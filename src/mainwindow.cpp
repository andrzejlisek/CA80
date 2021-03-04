#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

using namespace std;

//#define SoundBufSize 7000
//#define SoundTimerPeriod 20
//#define SoundChunkSize 1500
//#define SoundMinimalFill 1500

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    EmuRunning = false;

    // Ustawianie koloru tla i tekstu wyswietlacza
    QPalette p = ui->DiaplayScreen->palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    ui->DiaplayScreen->setPalette(p);

    QFont Font_ = ui->InfoMem->font();
    Font_.setPointSize(10);
    ui->InfoMem->setFont(Font_);
    Font_ = ui->InfoRegs->font();
    Font_.setPointSize(10);
    ui->InfoRegs->setFont(Font_);

    // Podlaczanie zdarzen zegara i klawiszy
    connect(ui->DiaplayScreen, SIGNAL(KeyPress(QKeyEvent*)), this, SLOT(KeyPress(QKeyEvent*)));
    connect(ui->DiaplayScreen, SIGNAL(KeyRelease(QKeyEvent*)), this, SLOT(KeyRelease(QKeyEvent*)));

    // Uniemozliwienie maksymalizacji okna
    this->setFixedSize(this->size());

    // Odczyt konfiguracji
    SetSettingsAllowed = false;

    ui->Mem0Size->setCurrentIndex(Core._CpuMem->MemSizeIdxGet(0));
    ui->Mem1Size->setCurrentIndex(Core._CpuMem->MemSizeIdxGet(1));
    ui->Mem2Size->setCurrentIndex(Core._CpuMem->MemSizeIdxGet(2));
    ui->Mem3Size->setCurrentIndex(Core._CpuMem->MemSizeIdxGet(3));

    ui->RomList->clear();
    for (uint I = 0; I < Core._CpuMem->RomFile.size(); I++)
    {
        ui->RomList->addItem(Eden::ToQStr(Core._CpuMem->GetRomFileAddr(I) + ": " + Core._CpuMem->RomFile[I]));
    }

    ui->NewCA80->setChecked(Core._Keyboard->NewCA80);
    ui->SoundAudible->setChecked(Core.SoundAudible);
    ui->NMITimeT->setText(Eden::ToQStr(to_string(Core._CpuMem->NMITime)));
    ui->INTTimeT->setText(Eden::ToQStr(to_string(Core._CpuMem->INTTime)));
    ui->NMITimeL->setChecked(Core._CpuMem->NMIExists);
    ui->INTTimeL->setChecked(Core._CpuMem->INTExists);

    ui->CTC0TimeL->setChecked(Core._CTC->CTC0Exists);
    ui->CTC0TimeT->setText(Eden::ToQStr(to_string(Core._CTC->CTC0Time)));
    ui->TimerPeriodT->setText(Eden::ToQStr(to_string(Core.TimerPeriod)));

    ui->SoundBufSizeT->setText(Eden::ToQStr(to_string(Core.SoundBufSize)));
    ui->SoundChunkSizeT->setText(Eden::ToQStr(to_string(Core.SoundChunkSize)));
    ui->SoundMinFillT->setText(Eden::ToQStr(to_string(Core.SoundMinFill)));
    ui->SoundVolumeT->setValue(Core.SoundVolume);
    ui->MusicVolumeT->setValue(Core.MusicVolume);
    ui->TapeVolumeT->setValue(Core.TapeVolume);
    ui->TapePrebufferT->setText(Eden::ToQStr(to_string(Core._TapeRecorder->TapeAudioPrebuffer)));
    ui->TriggerTimeT->setText(Eden::ToQStr(to_string(Core._Sound->TriggerTime)));

    GetKeyList();

    SetSettingsAllowed = true;

    TapeInfoCurrent = "";

    ui->ProgStart->setEnabled(true);
    ui->ProgStop->setEnabled(false);
    ui->InfoProgStart->setEnabled(true);
    ui->InfoProgStop->setEnabled(false);


    //int AddrI = Eden::HexToInt("3E04");
    //Core._CpuMem->Traps.push_back(AddrI);

}

MainWindow::~MainWindow()
{
    Core._CpuMem->ProgStop();
    Timer.stop();
    delete ui;
}

void MainWindow::EmuStart()
{
    ui->ProgStart->setEnabled(false);
    ui->InfoProgStart->setEnabled(false);
    ui->InfoProgStep->setEnabled(false);

    EmuRunning = true;
    SoundAudible = Core.SoundAudible;
    if (SoundAudible)
    {
        AP = new EdenClass::AudioPlayer();

        SoundChunkSize = Core.SoundChunkSize;
        SoundMinFill = Core.SoundMinFill;
        AP->SetParams(48000, 2, 1, Core.SoundBufSize, Core.TimerPeriod);
        AP->PutSilenceToBuffer(Core.SoundBufSize * 2);
        AP->PlayStart();
    }
    connect(&Timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    Timer.start(Core.TimerPeriod);
    Core._CpuMem->ProgStart(false);

    ui->ProgStop->setEnabled(true);
    ui->InfoProgStop->setEnabled(true);
}

void MainWindow::EmuStop()
{
    ui->ProgStop->setEnabled(false);
    ui->InfoProgStop->setEnabled(false);
    Core._CpuMem->ProgStop();
    ui->InfoProgStep->setEnabled(true);
}

void MainWindow::on_ProgStart_clicked()
{
    EmuStart();
}

void MainWindow::on_ProgStop_clicked()
{
    EmuStop();
}

void MainWindow::on_ProgReset_clicked()
{
    if (EmuRunning)
    {
        Core.Reset();
    }
    else
    {
        Core.Reset();
        TimerEvent();
    }
}

void MainWindow::AudioTimerTick()
{
    if (!EmuRunning)
    {
        return;
    }

    // Zabezpieczenie przed zwiekszaniem sie ilosci danych w buforze do odtworzenia,
    // ktore powodowaloby coraz wieksze opoznienie w stosunku do zadanych zmian
    // parametrow dzwieku
    while (AP->GetAudioRemaining() <= SoundMinFill)
    {
        // Tworzenie tymczasowej tablicy do dzwieku
        short * Temp = new short[SoundChunkSize];
        for (int i = 0; i < SoundChunkSize; i++)
        {
            Temp[i] = Core._Sound->GetSample(160 * Core.SoundVolume) + Core._Music->GetSample(160 * Core.MusicVolume) + Core._TapeRecorder->GetSample(160 * Core.TapeVolume);
        }

        // Wprowadzanie tablicy do bufora
        AP->PutToBuffer(Temp, SoundChunkSize);

        // Niszczenie tymczasowej tablicy
        delete[] Temp;
    }
}



void MainWindow::TimerEvent()
{
    Core._CpuMem->ExecMutex.lock();
    string NewDisp = Core._Display->GetDisplay();
    if (NewDisp.size() > 0)
    {
        ui->DiaplayScreen->setPlainText(Eden::ToQStr(NewDisp));
    }

    NewDisp = Core._TapeRecorder->GetInfo();
    if (TapeInfoCurrent != NewDisp)
    {
        ui->TapeInfo->setText(Eden::ToQStr(NewDisp));
        TapeInfoCurrent = NewDisp;
    }

    if (ui->InfoGet->isChecked())
    {
        ui->InfoRegs->setPlainText(Eden::ToQStr(Core._CpuMem->GetInfoRegs()));
        ui->InfoMem->setPlainText(Eden::ToQStr(Core._CpuMem->GetInfoMem(ui->InfomemAddr->value())));
    }

    if (EmuRunning && (!Core._CpuMem->ProgramWorking))
    {
        ui->ProgStop->setEnabled(false);
        ui->InfoProgStop->setEnabled(false);
        EmuRunning = false;
        if (SoundAudible)
        {
            QObject::disconnect(AP, SIGNAL(TimerTick(int)), this, SLOT(AudioTimerTick(int)));

            AP->PlayStop();
            delete AP;
        }
        else
        {
            Timer.stop();
            disconnect(&Timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
        }
        ui->ProgStart->setEnabled(true);
        ui->InfoProgStart->setEnabled(true);
        ui->InfoProgStep->setEnabled(true);
    }

    if (!Core._CpuMem->ExitMessage.empty())
    {
        ShowMessage(Core._CpuMem->ExitMessage, "");
        Core._CpuMem->ExitMessage = "";
    }

    PortO();

    if (SoundAudible)
    {
        AudioTimerTick();
    }

    Core._CpuMem->ExecMutex.unlock();
}

void MainWindow::on_Key00_pressed()
{
    Core._Keyboard->KeyPress(0);
}

void MainWindow::on_Key00_released()
{
    Core._Keyboard->KeyRelease(0);
}

void MainWindow::on_Key01_pressed()
{
    Core._Keyboard->KeyPress(1);
}

void MainWindow::on_Key01_released()
{
    Core._Keyboard->KeyRelease(1);
}

void MainWindow::on_Key02_pressed()
{
    Core._Keyboard->KeyPress(2);
}

void MainWindow::on_Key02_released()
{
    Core._Keyboard->KeyRelease(2);
}

void MainWindow::on_Key03_pressed()
{
    Core._Keyboard->KeyPress(3);
}

void MainWindow::on_Key03_released()
{
    Core._Keyboard->KeyRelease(3);
}

void MainWindow::on_Key04_pressed()
{
    Core._Keyboard->KeyPress(4);
}

void MainWindow::on_Key04_released()
{
    Core._Keyboard->KeyRelease(4);
}

void MainWindow::on_Key05_pressed()
{
    Core._Keyboard->KeyPress(5);
}

void MainWindow::on_Key05_released()
{
    Core._Keyboard->KeyRelease(5);
}

void MainWindow::on_Key06_pressed()
{
    Core._Keyboard->KeyPress(6);
}

void MainWindow::on_Key06_released()
{
    Core._Keyboard->KeyRelease(6);
}

void MainWindow::on_Key07_pressed()
{
    Core._Keyboard->KeyPress(7);
}

void MainWindow::on_Key07_released()
{
    Core._Keyboard->KeyRelease(7);
}

void MainWindow::on_Key08_pressed()
{
    Core._Keyboard->KeyPress(8);
}

void MainWindow::on_Key08_released()
{
    Core._Keyboard->KeyRelease(8);
}

void MainWindow::on_Key09_pressed()
{
    Core._Keyboard->KeyPress(9);
}

void MainWindow::on_Key09_released()
{
    Core._Keyboard->KeyRelease(9);
}

void MainWindow::on_Key0A_pressed()
{
    Core._Keyboard->KeyPress(10);
}

void MainWindow::on_Key0A_released()
{
    Core._Keyboard->KeyRelease(10);
}

void MainWindow::on_Key0B_pressed()
{
    Core._Keyboard->KeyPress(11);
}

void MainWindow::on_Key0B_released()
{
    Core._Keyboard->KeyRelease(11);
}

void MainWindow::on_Key0C_pressed()
{
    Core._Keyboard->KeyPress(12);
}

void MainWindow::on_Key0C_released()
{
    Core._Keyboard->KeyRelease(12);
}

void MainWindow::on_Key0D_pressed()
{
    Core._Keyboard->KeyPress(13);
}

void MainWindow::on_Key0D_released()
{
    Core._Keyboard->KeyRelease(13);
}

void MainWindow::on_Key0E_pressed()
{
    Core._Keyboard->KeyPress(14);
}

void MainWindow::on_Key0E_released()
{
    Core._Keyboard->KeyRelease(14);
}

void MainWindow::on_Key0F_pressed()
{
    Core._Keyboard->KeyPress(15);
}

void MainWindow::on_Key0F_released()
{
    Core._Keyboard->KeyRelease(15);
}

void MainWindow::on_Key10_pressed()
{
    Core._Keyboard->KeyPress(16);
}

void MainWindow::on_Key10_released()
{
    Core._Keyboard->KeyRelease(16);
}

void MainWindow::on_Key11_pressed()
{
    Core._Keyboard->KeyPress(17);
}

void MainWindow::on_Key11_released()
{
    Core._Keyboard->KeyRelease(17);
}

void MainWindow::on_Key12_pressed()
{
    Core._Keyboard->KeyPress(18);
}

void MainWindow::on_Key12_released()
{
    Core._Keyboard->KeyRelease(18);
}

void MainWindow::on_Key13_pressed()
{
    Core._Keyboard->KeyPress(19);
}

void MainWindow::on_Key13_released()
{
    Core._Keyboard->KeyRelease(19);
}

void MainWindow::on_Key14_pressed()
{
    Core._Keyboard->KeyPress(20);
}

void MainWindow::on_Key14_released()
{
    Core._Keyboard->KeyRelease(20);
}

void MainWindow::on_Key15_pressed()
{
    Core._Keyboard->KeyPress(21);
}

void MainWindow::on_Key15_released()
{
    Core._Keyboard->KeyRelease(21);
}

void MainWindow::on_Key16_pressed()
{
    Core._Keyboard->KeyPress(22);
}

void MainWindow::on_Key16_released()
{
    Core._Keyboard->KeyRelease(22);
}

void MainWindow::on_Key17_pressed()
{
    Core._Keyboard->KeyPress(23);
}

void MainWindow::on_Key17_released()
{
    Core._Keyboard->KeyRelease(23);
}

void MainWindow::KeyPress(QKeyEvent * event)
{
    if (!event->isAutoRepeat())
    {
        ui->KeyCodeT->setText(Eden::ToQStr(to_string(event->key())));
        if (Core.KeyMap.find(event->key()) != Core.KeyMap.end())
        {
            switch(Core.KeyMap[event->key()])
            {
                case 0x00: ButtonPress(ui->Key00); break;
                case 0x01: ButtonPress(ui->Key01); break;
                case 0x02: ButtonPress(ui->Key02); break;
                case 0x03: ButtonPress(ui->Key03); break;
                case 0x04: ButtonPress(ui->Key04); break;
                case 0x05: ButtonPress(ui->Key05); break;
                case 0x06: ButtonPress(ui->Key06); break;
                case 0x07: ButtonPress(ui->Key07); break;
                case 0x08: ButtonPress(ui->Key08); break;
                case 0x09: ButtonPress(ui->Key09); break;
                case 0x0A: ButtonPress(ui->Key0A); break;
                case 0x0B: ButtonPress(ui->Key0B); break;
                case 0x0C: ButtonPress(ui->Key0C); break;
                case 0x0D: ButtonPress(ui->Key0D); break;
                case 0x0E: ButtonPress(ui->Key0E); break;
                case 0x0F: ButtonPress(ui->Key0F); break;
                case 0x10: ButtonPress(ui->Key10); break;
                case 0x11: ButtonPress(ui->Key11); break;
                case 0x12: ButtonPress(ui->Key12); break;
                case 0x13: ButtonPress(ui->Key13); break;
                case 0x14: ButtonPress(ui->Key14); break;
                case 0x15: ButtonPress(ui->Key15); break;
                case 0x16: ButtonPress(ui->Key16); break;
                case 0x17: ButtonPress(ui->Key17); break;
            }
        }
    }
}

void MainWindow::KeyRelease(QKeyEvent * event)
{
    if (!event->isAutoRepeat())
    {
        if (Core.KeyMap.find(event->key()) != Core.KeyMap.end())
        {
            switch(Core.KeyMap[event->key()])
            {
                case 0x00: ButtonRelease(ui->Key00); break;
                case 0x01: ButtonRelease(ui->Key01); break;
                case 0x02: ButtonRelease(ui->Key02); break;
                case 0x03: ButtonRelease(ui->Key03); break;
                case 0x04: ButtonRelease(ui->Key04); break;
                case 0x05: ButtonRelease(ui->Key05); break;
                case 0x06: ButtonRelease(ui->Key06); break;
                case 0x07: ButtonRelease(ui->Key07); break;
                case 0x08: ButtonRelease(ui->Key08); break;
                case 0x09: ButtonRelease(ui->Key09); break;
                case 0x0A: ButtonRelease(ui->Key0A); break;
                case 0x0B: ButtonRelease(ui->Key0B); break;
                case 0x0C: ButtonRelease(ui->Key0C); break;
                case 0x0D: ButtonRelease(ui->Key0D); break;
                case 0x0E: ButtonRelease(ui->Key0E); break;
                case 0x0F: ButtonRelease(ui->Key0F); break;
                case 0x10: ButtonRelease(ui->Key10); break;
                case 0x11: ButtonRelease(ui->Key11); break;
                case 0x12: ButtonRelease(ui->Key12); break;
                case 0x13: ButtonRelease(ui->Key13); break;
                case 0x14: ButtonRelease(ui->Key14); break;
                case 0x15: ButtonRelease(ui->Key15); break;
                case 0x16: ButtonRelease(ui->Key16); break;
                case 0x17: ButtonRelease(ui->Key17); break;
            }
        }
    }
}

void MainWindow::ButtonPress(QPushButton *Btn)
{
    Btn->setCheckable(true);
    Btn->setChecked(true);
    Btn->pressed();
}

void MainWindow::ButtonRelease(QPushButton *Btn)
{
    Btn->setChecked(false);
    Btn->setCheckable(false);
    Btn->released();
}

void MainWindow::on_TapeStop_clicked()
{
    Core._TapeRecorder->SetState(0);
}

void MainWindow::on_TapeFwd_clicked()
{
    Core._TapeRecorder->SetState(3);
}

void MainWindow::on_TapeRew_clicked()
{
    Core._TapeRecorder->SetState(4);
}

void MainWindow::on_TapePlay_clicked()
{
    Core._TapeRecorder->SetState(1);
}

void MainWindow::on_TapeRecord_clicked()
{
    Core._TapeRecorder->SetState(2);
}

void MainWindow::on_TapeGen_clicked()
{
    if (Core._TapeRecorder->IsIdle())
    {
        string LS = InputBox("Podaj długość taśmy w sekundach", "", "60");
        string ResX = InputBox("Podaj liczbę cykli zegarowych na próbkę", "", "1000");
        if (!LS.empty())
        {
            llong Len = stoll(LS);
            llong Res = stoll(ResX);
            Len = Len * 4000000;
            //Len = Len / Res;
            if (Res > 0)
            {
                Core._TapeRecorder->TapeGenerate(Len, Res);
            }
        }
    }
}

void MainWindow::on_TapeLoad_clicked()
{
    if (Core._TapeRecorder->IsIdle())
    {
        QString FN = QFileDialog::getOpenFileName(this, "", LastPath, "Files (*.*)");
        SaveLastPath(FN);
        if (!FN.isEmpty())
        {
            Core._TapeRecorder->FileLoad(Eden::ToStr(FN));
        }
    }
}

void MainWindow::on_TapeSave_clicked()
{
    if (Core._TapeRecorder->IsIdle())
    {
        QString FN = QFileDialog::getSaveFileName(this, "", LastPath, "Files (*.*)");
        SaveLastPath(FN);
        if (!FN.isEmpty())
        {
            Core._TapeRecorder->FileSave(Eden::ToStr(FN));
        }
    }
}

void MainWindow::on_InfomemAddr_valueChanged(int value)
{
    ui->InfoMemPanel->setTitle(Eden::ToQStr("Pamięć " + Eden::IntToHex8(value) + "00-" + Eden::IntToHex8(value) + "FF"));
    if (!EmuRunning)
    {
        TimerEvent();
    }
}

void MainWindow::on_InfoProgStart_clicked()
{
    EmuStart();
}

void MainWindow::on_InfoProgStop_clicked()
{
    EmuStop();
}

void MainWindow::on_InfoProgStep_clicked()
{
    Core._CpuMem->ProgStart(true);
    if (!EmuRunning)
    {
        TimerEvent();
    }
}

void MainWindow::SetSettings()
{
    if (SetSettingsAllowed)
    {
        Core._Keyboard->NewCA80 = ui->NewCA80->isChecked();
        Core.SoundAudible = ui->SoundAudible->isChecked();
        Core.SoundVolume = ui->SoundVolumeT->value();
        Core.TapeVolume = ui->TapeVolumeT->value();
        Core._TapeRecorder->TapeAudioPrebuffer = Eden::ToInt(ui->TapePrebufferT->text());
        Core._Sound->TriggerTime = Eden::ToInt(ui->TriggerTimeT->text());
        Core.MusicVolume = ui->MusicVolumeT->value();
        Core._CpuMem->MemSizeIdxSet(0, ui->Mem0Size->currentIndex());
        Core._CpuMem->MemSizeIdxSet(1, ui->Mem1Size->currentIndex());
        Core._CpuMem->MemSizeIdxSet(2, ui->Mem2Size->currentIndex());
        Core._CpuMem->MemSizeIdxSet(3, ui->Mem3Size->currentIndex());
        Core._CpuMem->NMITime = Eden::ToInt(ui->NMITimeT->text());
        Core._CpuMem->INTTime = Eden::ToInt(ui->INTTimeT->text());
        Core._CpuMem->NMIExists = ui->NMITimeL->isChecked();
        Core._CpuMem->INTExists = ui->INTTimeL->isChecked();
        Core._CTC->CTC0Exists = ui->CTC0TimeL->isChecked();
        Core._CTC->CTC0Time = Eden::ToInt(ui->CTC0TimeT->text());
        Core.TimerPeriod = Eden::ToInt(ui->TimerPeriodT->text());
        Core.SoundBufSize = Eden::ToInt(ui->SoundBufSizeT->text());
        Core.SoundChunkSize = Eden::ToInt(ui->SoundChunkSizeT->text());
        Core.SoundMinFill = Eden::ToInt(ui->SoundMinFillT->text());
    }
}

void MainWindow::on_NewCA80_toggled(bool checked)
{
    checked = !checked;
    SetSettings();
}

void MainWindow::on_SoundAudible_toggled(bool checked)
{
    checked = !checked;
    SetSettings();
}

string MainWindow::InputBox(string Query, string Title, string Default)
{
    bool OK;
    QString X = QInputDialog::getText(this, Eden::ToQStr(Title), Eden::ToQStr(Query), QLineEdit::Normal, Eden::ToQStr(Default), &OK);
    if (OK)
    {
        return Eden::ToStr(X);
    }
    else
    {
        return "";
    }
}

void MainWindow::ShowMessage(string Message, string Title)
{
    QMessageBox Msg;
    if (Title == "")
    {
        Msg.setWindowTitle(" ");
    }
    else
    {
        Msg.setWindowTitle(Eden::ToQStr(Title));
    }
    Msg.setText(Eden::ToQStr(Message));
    Msg.exec();
}

void MainWindow::on_TrapAdd_clicked()
{
    string Addr = InputBox("Podaj adres pułapki w formacie hex", "", "0000");
    if (Addr.length() == 4)
    {
        int AddrI = Eden::HexToInt(Addr);
        if (!(std::find(Core._CpuMem->Traps.begin(), Core._CpuMem->Traps.end(), AddrI) != Core._CpuMem->Traps.end()))
        {
            Core._CpuMem->Traps.push_back(AddrI);
            Core._CpuMem->Traps.sort();

            ui->TrapList->clear();
            for (list<int>::iterator It = Core._CpuMem->Traps.begin(); It != Core._CpuMem->Traps.end(); It++)
            {
                ui->TrapList->addItem(Eden::ToQStr(Eden::IntToHex16(*It)));
            }
        }
    }
}

void MainWindow::on_TrapRem_clicked()
{
    if (ui->TrapList->currentRow() >= 0)
    {
        int T = ui->TrapList->currentRow();
        list<int>::iterator It = Core._CpuMem->Traps.begin();
        while (T > 0)
        {
            T--;
            It++;
        }
        Core._CpuMem->Traps.erase(It);

        ui->TrapList->clear();
        for (It = Core._CpuMem->Traps.begin(); It != Core._CpuMem->Traps.end(); It++)
        {
            ui->TrapList->addItem(Eden::ToQStr(Eden::IntToHex16(*It)));
        }
    }
}

void MainWindow::on_RomAdd_clicked()
{
    string Addr = InputBox("Podaj adres początkowy w formacie hex", "", "0000");
    if (Addr.length() == 4)
    {
        QString FN = QFileDialog::getOpenFileName(this, "", LastPath, "Files (*.*)");
        SaveLastPath(FN);
        if (!FN.isEmpty())
        {
            Core._CpuMem->RomFile.push_back(Eden::ToStr(FN));
            Core._CpuMem->RomBegin.push_back(Eden::HexToInt(Addr));
            Core._CpuMem->RomEnd.push_back(-1);
            Core._CpuMem->SortRoms();
            Core._CpuMem->LoadRom(false);

            ui->RomList->clear();
            for (uint I = 0; I < Core._CpuMem->RomFile.size(); I++)
            {
                ui->RomList->addItem(Eden::ToQStr(Core._CpuMem->GetRomFileAddr(I) + ": " + Core._CpuMem->RomFile[I]));
            }
        }
    }
}

void MainWindow::on_RomRem_clicked()
{
    if (ui->RomList->currentRow() >= 0)
    {
        vector<int>::iterator VItI1;
        vector<int>::iterator VItI2;
        vector<string>::iterator VItS;
        int T;
        T = ui->RomList->currentRow();
        VItS = Core._CpuMem->RomFile.begin();
        VItI1 = Core._CpuMem->RomBegin.begin();
        VItI2 = Core._CpuMem->RomEnd.begin();
        while (T > 0)
        {
            VItS++;
            VItI1++;
            VItI2++;
            T--;
        }
        Core._CpuMem->RomFile.erase(VItS);
        Core._CpuMem->RomBegin.erase(VItI1);
        Core._CpuMem->RomEnd.erase(VItI2);

        ui->RomList->clear();
        for (uint I = 0; I < Core._CpuMem->RomFile.size(); I++)
        {
            ui->RomList->addItem(Eden::ToQStr(Core._CpuMem->GetRomFileAddr(I) + ": " + Core._CpuMem->RomFile[I]));
        }
    }
}

void MainWindow::SaveLastPath(QString X)
{
    if (!X.isEmpty())
    {
         LastPath = QFileInfo(X).path();
    }
}

void MainWindow::GetKeyList()
{
    ui->KeyList->clear();
    map<int, int>::iterator MIt;
    for (MIt = Core.KeyMap.begin(); MIt != Core.KeyMap.end(); MIt++)
    {
        string KeyItem = to_string(MIt->first) + " -> [" + Core.VisKey[MIt->second] + "]";
        ui->KeyList->addItem(Eden::ToQStr(KeyItem));
    }
}

void MainWindow::on_KeyAdd_clicked()
{
    if (ui->KeyIdT->text().length() == 1)
    {
        string Id = Eden::ToStr(ui->KeyIdT->text().toUpper());

        int T = -1;
        for (int I = 0; I < 24; I++)
        {
            if (Core.VisKey[I] == Id)
            {
                T = I;
            }
        }

        if ((T >= 0) && (T < 24))
        {
            if (ui->KeyCodeT->text() != "")
            {
                Core.KeyMap[stoi(Eden::ToStr(ui->KeyCodeT->text()))] = T;
                GetKeyList();
            }
        }
    }
}

void MainWindow::on_KeyRem_clicked()
{
    if (ui->KeyList->currentRow() >= 0)
    {
        map<int, int>::iterator MIt = Core.KeyMap.begin();
        int T = ui->KeyList->currentRow();
        while (T > 0)
        {
            MIt++;
            T--;
        }
        Core.KeyMap.erase(MIt->first);

        GetKeyList();
    }
}

void MainWindow::on_MemLoad_clicked()
{
    string Addr1 = InputBox("Podaj adres początkowy w formacie hex", "", "C000");
    if (Addr1.length() == 4)
    {
        QString FN = QFileDialog::getOpenFileName(this, "", LastPath, "Files (*.*)");
        SaveLastPath(FN);
        if (!FN.isEmpty())
        {
            Core._CpuMem->MemImport(Eden::ToStr(FN), Eden::HexToInt(Addr1));
        }
    }
}

void MainWindow::on_MemSave_clicked()
{
    string Addr1 = InputBox("Podaj adres początkowy w formacie hex", "", "C000");
    if (Addr1.length() == 4)
    {
        string Addr2 = InputBox("Podaj adres końcowy w formacie hex", "", "DFFF");
        if (Addr2.length() == 4)
        {
            QString FN = QFileDialog::getSaveFileName(this, "", LastPath, "Files (*.*)");
            SaveLastPath(FN);
            if (!FN.isEmpty())
            {
                Core._CpuMem->MemExport(Eden::ToStr(FN), Eden::HexToInt(Addr1), Eden::HexToInt(Addr2));
            }
        }
    }
}

void MainWindow::on_Mem0Size_currentIndexChanged(int index)
{
    index++;
    index--;
    SetSettings();
}

void MainWindow::on_Mem1Size_currentIndexChanged(int index)
{
    index++;
    index--;
    SetSettings();
}

void MainWindow::on_Mem2Size_currentIndexChanged(int index)
{
    index++;
    index--;
    SetSettings();
}

void MainWindow::on_Mem3Size_currentIndexChanged(int index)
{
    index++;
    index--;
    SetSettings();
}

void MainWindow::on_NMITimeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_TimerPeriodT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_SoundBufSizeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_SoundChunkSizeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_InfoGet_toggled(bool checked)
{
    if (!EmuRunning)
    {
        checked = !checked;
        TimerEvent();
    }
}

void MainWindow::on_SoundVolumeT_valueChanged(int value)
{
    value++;
    value--;
    SetSettings();
}

void MainWindow::on_INTTimeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_MusicVolumeT_valueChanged(int value)
{
    value++;
    value--;
    SetSettings();
}

void MainWindow::on_TriggerTimeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_NMITimeL_toggled(bool checked)
{
    checked = !checked;
    checked = !checked;
    SetSettings();
}

void MainWindow::on_INTTimeL_toggled(bool checked)
{
    checked = !checked;
    checked = !checked;
    SetSettings();
}


void MainWindow::PortI(bool Dummy)
{
    Dummy = !Dummy;
    int XA0 = ui->PortA0->isChecked() ?   1 : 0;
    int XA1 = ui->PortA1->isChecked() ?   2 : 0;
    int XA2 = ui->PortA2->isChecked() ?   4 : 0;
    int XA3 = ui->PortA3->isChecked() ?   8 : 0;
    int XA4 = ui->PortA4->isChecked() ?  16 : 0;
    int XA5 = ui->PortA5->isChecked() ?  32 : 0;
    int XA6 = ui->PortA6->isChecked() ?  64 : 0;
    int XA7 = ui->PortA7->isChecked() ? 128 : 0;
    int XB0 = ui->PortB0->isChecked() ?   1 : 0;
    int XB1 = ui->PortB1->isChecked() ?   2 : 0;
    int XB2 = ui->PortB2->isChecked() ?   4 : 0;
    int XB3 = ui->PortB3->isChecked() ?   8 : 0;
    int XB4 = ui->PortB4->isChecked() ?  16 : 0;
    int XB5 = ui->PortB5->isChecked() ?  32 : 0;
    int XB6 = ui->PortB6->isChecked() ?  64 : 0;
    int XB7 = ui->PortB7->isChecked() ? 128 : 0;
    int XC0 = ui->PortC0->isChecked() ?   1 : 0;
    int XC1 = ui->PortC1->isChecked() ?   2 : 0;
    int XC2 = ui->PortC2->isChecked() ?   4 : 0;
    int XC3 = ui->PortC3->isChecked() ?   8 : 0;
    int XC4 = ui->PortC4->isChecked() ?  16 : 0;
    int XC5 = ui->PortC5->isChecked() ?  32 : 0;
    int XC6 = ui->PortC6->isChecked() ?  64 : 0;
    int XC7 = ui->PortC7->isChecked() ? 128 : 0;
    if (Core._I8255_USR->ParAisInput)
    {
        Core._I8255_USR->DevASet(XA0 + XA1 + XA2 + XA3 + XA4 + XA5 + XA6 + XA7);
    }
    if (Core._I8255_USR->ParBisInput)
    {
        Core._I8255_USR->DevBSet(XB0 + XB1 + XB2 + XB3 + XB4 + XB5 + XB6 + XB7);
    }
    if (Core._I8255_USR->ParCHisInput)
    {
        Core._I8255_USR->DevCHSet(XC0 + XC1 + XC2 + XC3 + XC4 + XC5 + XC6 + XC7);
    }
    if (Core._I8255_USR->ParCLisInput)
    {
        Core._I8255_USR->DevCLSet(XC0 + XC1 + XC2 + XC3 + XC4 + XC5 + XC6 + XC7);
    }
}

void MainWindow::PortO()
{
    uchar XA = Core._I8255_USR->DevAGet();
    uchar XB = Core._I8255_USR->DevBGet();
    uchar XC = Core._I8255_USR->DevCGet();
    if (Core._I8255_USR->ParAisInput)
    {
        ui->PortA0->setText("< A0");
        ui->PortA1->setText("< A1");
        ui->PortA2->setText("< A2");
        ui->PortA3->setText("< A3");
        ui->PortA4->setText("< A4");
        ui->PortA5->setText("< A5");
        ui->PortA6->setText("< A6");
        ui->PortA7->setText("< A7");
    }
    else
    {
        ui->PortA0->setText("> A0");
        ui->PortA1->setText("> A1");
        ui->PortA2->setText("> A2");
        ui->PortA3->setText("> A3");
        ui->PortA4->setText("> A4");
        ui->PortA5->setText("> A5");
        ui->PortA6->setText("> A6");
        ui->PortA7->setText("> A7");
        ui->PortA0->setChecked(XA & b00000001);
        ui->PortA1->setChecked(XA & b00000010);
        ui->PortA2->setChecked(XA & b00000100);
        ui->PortA3->setChecked(XA & b00001000);
        ui->PortA4->setChecked(XA & b00010000);
        ui->PortA5->setChecked(XA & b00100000);
        ui->PortA6->setChecked(XA & b01000000);
        ui->PortA7->setChecked(XA & b10000000);
    }
    if (Core._I8255_USR->ParBisInput)
    {
        ui->PortB0->setText("< B0");
        ui->PortB1->setText("< B1");
        ui->PortB2->setText("< B2");
        ui->PortB3->setText("< B3");
        ui->PortB4->setText("< B4");
        ui->PortB5->setText("< B5");
        ui->PortB6->setText("< B6");
        ui->PortB7->setText("< B7");
    }
    else
    {
        ui->PortB0->setText("> B0");
        ui->PortB1->setText("> B1");
        ui->PortB2->setText("> B2");
        ui->PortB3->setText("> B3");
        ui->PortB4->setText("> B4");
        ui->PortB5->setText("> B5");
        ui->PortB6->setText("> B6");
        ui->PortB7->setText("> B7");
        ui->PortB0->setChecked(XB & b00000001);
        ui->PortB1->setChecked(XB & b00000010);
        ui->PortB2->setChecked(XB & b00000100);
        ui->PortB3->setChecked(XB & b00001000);
        ui->PortB4->setChecked(XB & b00010000);
        ui->PortB5->setChecked(XB & b00100000);
        ui->PortB6->setChecked(XB & b01000000);
        ui->PortB7->setChecked(XB & b10000000);
    }
    if (Core._I8255_USR->ParCLisInput)
    {
        ui->PortC0->setText("< C0");
        ui->PortC1->setText("< C1");
        ui->PortC2->setText("< C2");
        ui->PortC3->setText("< C3");
    }
    else
    {
        ui->PortC0->setText("> C0");
        ui->PortC1->setText("> C1");
        ui->PortC2->setText("> C2");
        ui->PortC3->setText("> C3");
        ui->PortC0->setChecked(XC & b00000001);
        ui->PortC1->setChecked(XC & b00000010);
        ui->PortC2->setChecked(XC & b00000100);
        ui->PortC3->setChecked(XC & b00001000);
    }
    if (Core._I8255_USR->ParCHisInput)
    {
        ui->PortC4->setText("< C4");
        ui->PortC5->setText("< C5");
        ui->PortC6->setText("< C6");
        ui->PortC7->setText("< C7");
    }
    else
    {
        ui->PortC4->setText("> C4");
        ui->PortC5->setText("> C5");
        ui->PortC6->setText("> C6");
        ui->PortC7->setText("> C7");
        ui->PortC4->setChecked(XC & b00010000);
        ui->PortC5->setChecked(XC & b00100000);
        ui->PortC6->setChecked(XC & b01000000);
        ui->PortC7->setChecked(XC & b10000000);
    }
}

void MainWindow::on_PortA7_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA6_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA5_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA4_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA3_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA2_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA1_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortA0_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB7_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB6_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB5_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB4_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB3_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB2_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB1_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortB0_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC7_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC6_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC5_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC4_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC3_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC2_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC1_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_PortC0_toggled(bool checked)
{
    PortI(checked);
}

void MainWindow::on_SoundMinFillT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_TapePrebufferT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_TapeVolumeT_valueChanged(int value)
{
    value++;
    value--;
    SetSettings();
}

void MainWindow::on_CTC0TimeT_textChanged(const QString &arg1)
{
    QString X = arg1;
    X = "";
    SetSettings();
}

void MainWindow::on_CTC0TimeL_toggled(bool checked)
{
    checked = !checked;
    checked = !checked;
    SetSettings();
}
