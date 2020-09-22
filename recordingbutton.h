#ifndef RECORDINGBUTTON_H
#define RECORDINGBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class RecordingButton : public QObject
{
    Q_OBJECT
public:
    explicit RecordingButton(QObject *parent = nullptr);

signals:


public slots:



private:
    QPushButton *pauseButton;
    QPushButton *resumeButton;
    QVBoxLayout *layout;
};

#endif // RECORDINGBUTTON_H
