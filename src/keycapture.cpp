#include "keycapture.h"

KeyCapture::KeyCapture(QWidget *parent) :
    QPlainTextEdit(parent)
{

}

void KeyCapture::keyPressEvent(QKeyEvent * event)
{
    emit KeyPress(event);
}

void KeyCapture::keyReleaseEvent(QKeyEvent * event)
{
    emit KeyRelease(event);
}
