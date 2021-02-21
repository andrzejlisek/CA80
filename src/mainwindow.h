#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "appcore.h"
#include <QTimer>
#include "eden.h"
#include <QPushButton>
#include "audioplayer.h"
#include <cmath>
#include "binary.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppCore Core;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void AudioTimerTick();

    void KeyPress(QKeyEvent *event);
    void KeyRelease(QKeyEvent *event);


    void TimerEvent();
    void on_ProgStart_clicked();
    void on_ProgStop_clicked();
    void on_ProgReset_clicked();

    void on_Key00_pressed();

    void on_Key00_released();

    void on_Key01_pressed();

    void on_Key01_released();

    void on_Key02_pressed();

    void on_Key02_released();

    void on_Key03_pressed();

    void on_Key03_released();

    void on_Key04_pressed();

    void on_Key04_released();

    void on_Key05_pressed();

    void on_Key05_released();

    void on_Key06_pressed();

    void on_Key06_released();

    void on_Key07_pressed();

    void on_Key07_released();

    void on_Key08_pressed();

    void on_Key08_released();

    void on_Key09_pressed();

    void on_Key09_released();

    void on_Key0A_pressed();

    void on_Key0A_released();

    void on_Key0B_pressed();

    void on_Key0B_released();

    void on_Key0C_pressed();

    void on_Key0C_released();

    void on_Key0D_pressed();

    void on_Key0D_released();

    void on_Key0E_pressed();

    void on_Key0E_released();

    void on_Key0F_pressed();

    void on_Key0F_released();

    void on_Key10_pressed();

    void on_Key10_released();

    void on_Key11_pressed();

    void on_Key11_released();

    void on_Key12_pressed();

    void on_Key12_released();

    void on_Key13_pressed();

    void on_Key13_released();

    void on_Key14_pressed();

    void on_Key14_released();

    void on_Key15_pressed();

    void on_Key15_released();

    void on_Key16_pressed();

    void on_Key16_released();

    void on_Key17_pressed();

    void on_Key17_released();

    void on_TapeStop_clicked();

    void on_TapeFwd_clicked();

    void on_TapeRew_clicked();

    void on_TapePlay_clicked();

    void on_TapeRecord_clicked();

    void on_TapeGen_clicked();

    void on_TapeLoad_clicked();

    void on_TapeSave_clicked();

    void on_InfomemAddr_valueChanged(int value);

    void on_InfoProgStart_clicked();

    void on_InfoProgStop_clicked();

    void on_InfoProgStep_clicked();

    void on_NewCA80_toggled(bool checked);

    void on_SoundAudible_toggled(bool checked);

    void on_Mem0Writable_toggled(bool checked);

    void on_Mem1Writable_toggled(bool checked);

    void on_Mem2Writable_toggled(bool checked);

    void on_Mem3Writable_toggled(bool checked);

    void on_TrapAdd_clicked();

    void on_TrapRem_clicked();

    void on_RomAdd_clicked();

    void on_RomRem_clicked();

    void on_KeyAdd_clicked();

    void on_KeyRem_clicked();

    void on_MemLoad_clicked();

    void on_MemSave_clicked();

    void on_Mem0Size_currentIndexChanged(int index);

    void on_Mem1Size_currentIndexChanged(int index);

    void on_Mem2Size_currentIndexChanged(int index);

    void on_Mem3Size_currentIndexChanged(int index);

    void on_NMITimeT_textChanged(const QString &arg1);

    void on_TimerPeriodT_textChanged(const QString &arg1);

    void on_SoundBufSizeT_textChanged(const QString &arg1);

    void on_SoundChunkSizeT_textChanged(const QString &arg1);

    void on_InfoGet_toggled(bool checked);

    void on_SoundVolumeT_valueChanged(int value);

    void on_INTTimeT_textChanged(const QString &arg1);

    void on_MusicVolumeT_valueChanged(int value);

    void on_TriggerTimeT_textChanged(const QString &arg1);

    void on_NMITimeL_toggled(bool checked);

    void on_INTTimeL_toggled(bool checked);

    void on_PortA7_toggled(bool checked);

    void on_PortA6_toggled(bool checked);

    void on_PortA5_toggled(bool checked);

    void on_PortA4_toggled(bool checked);

    void on_PortA3_toggled(bool checked);

    void on_PortA2_toggled(bool checked);

    void on_PortA1_toggled(bool checked);

    void on_PortA0_toggled(bool checked);

    void on_PortB7_toggled(bool checked);

    void on_PortB6_toggled(bool checked);

    void on_PortB5_toggled(bool checked);

    void on_PortB4_toggled(bool checked);

    void on_PortB3_toggled(bool checked);

    void on_PortB2_toggled(bool checked);

    void on_PortB1_toggled(bool checked);

    void on_PortB0_toggled(bool checked);

    void on_PortC7_toggled(bool checked);

    void on_PortC6_toggled(bool checked);

    void on_PortC5_toggled(bool checked);

    void on_PortC4_toggled(bool checked);

    void on_PortC3_toggled(bool checked);

    void on_PortC2_toggled(bool checked);

    void on_PortC1_toggled(bool checked);

    void on_PortC0_toggled(bool checked);

private:
    int SoundChunkSize;
    bool EmuRunning;
    bool SoundAudible;
    void EmuStart();
    void EmuStop();
    EdenClass::AudioPlayer * AP;
    Ui::MainWindow *ui;
    QTimer Timer;
    void ButtonPress(QPushButton *Btn);
    void ButtonRelease(QPushButton *Btn);
    string TapeInfoCurrent;
    bool SetSettingsAllowed;
    void SetSettings();
    string InputBox(string Query, string Title, string Default);
    void ShowMessage(string Message, string Title);
    QString LastPath;
    void SaveLastPath(QString X);
    void GetKeyList();

    void PortI(bool Dummy);
    void PortO();
};

#endif // MAINWINDOW_H
