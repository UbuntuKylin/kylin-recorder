#ifndef RECORDING_H
#define RECORDING_H

#include <QObject>
#include <QFile>
#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QSlider>
#include <QToolButton>
#include "mywave.h"
#include "mainwindow.h"
#include <QDebug>
class Recording : public QObject
{
    Q_OBJECT
public:
    explicit Recording(QObject *parent = nullptr);

public://音频相关

    QFile *file;
    QAudioInput * m_audioInput;//1
    QAudioOutput* output;//1
    short *outdata;
    qint64 len;
    QList<int> maxNum;//存储振幅的大小的整型列表
    QAudioFormat mFormatSound;
    QAudioInput *mpAudioInputSound;		// 负责监听声音
    QAudioOutput *mpAudioOutputSound;
    QVector<myWave*> mywave;
    QTimer *updateTimer31;
    int value;
    int miVolume;
    int miMaxValue;
    QIODevice *mpInputDevSound;
    QIODevice *mpOutputDevSound;
    //时间相关
    QTimer *pTimer;//1
    QTime baseTime;//1
    QTimer *my_time;
    //显示的时间
    QString timeStr;
public :
    int i=0;
    void CreateAudioInput();
    void CreateAudioOutput();
    void record_press();
    int ApplyVolumeToSample(short iSample);

public slots://子线程的槽函数
    void OnReadMore();
    void test();


};

#endif // RECORDING_H
