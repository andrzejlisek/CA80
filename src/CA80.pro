#-------------------------------------------------
#
# Project created by QtCreator 2015-01-07T02:47:57
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CA80
TEMPLATE = app


SOURCES += main.cpp\
    configfile.cpp \
    ctc.cpp \
        mainwindow.cpp \
    appcore.cpp \
    cpumem.cpp \
    display.cpp \
    keyboard.cpp \
    i8255.cpp \
    keycapture.cpp \
    sound.cpp \
    taperecorder.cpp \
    extdatetime.cpp \
    i8255_sys.cpp \
    i8255_usr.cpp \
    music.cpp \
    eden.cpp \
    audioplayer.cpp \
    audioplayerthread.cpp

HEADERS  += mainwindow.h \
    appcore.h \
    configfile.h \
    cpumem.h \
    ctc.h \
    display.h \
    keyboard.h \
    i8255.h \
    keycapture.h \
    sound.h \
    taperecorder.h \
    extdatetime.h \
    i8255_sys.h \
    i8255_usr.h \
    music.h \
    binary.h \
    eden.h \
    audioplayer.h \
    audioplayerthread.h \
    objmem.h

FORMS    += mainwindow.ui

CONFIG += c++11
