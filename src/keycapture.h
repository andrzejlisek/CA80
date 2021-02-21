#ifndef KEYCAPTURE_H
#define KEYCAPTURE_H

#include <QPlainTextEdit>

class KeyCapture : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit KeyCapture(QWidget *parent = 0);

signals:
    void KeyPress(QKeyEvent *event);
    void KeyRelease(QKeyEvent *event);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);


public slots:

};

#endif // KEYCAPTURE_H
