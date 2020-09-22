#ifndef MYTHREAD_H
#define MYTHREAD_H
//===================
#include <QWidget>
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QThread>

#include <QMultimedia>
#include <QFile>
#include <QDebug>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QTimer>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QAudio>

#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QAudioRecorder>
#include <QFile>
#include <QIODevice>
#include "settings.h"
#include "save.h"
#include "wavfileheader.h"
//==================

#define BufferSize     35280
#define rectangleCount 40//矩形条个数
class MyThread : public QMainWindow
{
    Q_OBJECT
public:
    explicit MyThread(QWidget *parent = nullptr);

    Settings set;
    Save saveas;
    QString fileName;
    QString daultfileName;
    QString desktop_path;
    int i=0;
signals:
    void recordPaint(int);
    void stopRecord();
    void changeVoicePicture();

private:
    qint64 addWavHeader(QString catheFileName , QString wavFileName);
    QFile *file;
    QAudioInput * audioInputFile;//1将音频输入文件
    //        QAudioOutput* output;//1
    short *outdata;



    qint64 len;
    QAudioFormat mFormatSound;
    QAudioInput *audioInputSound;		// 负责监听声音
    QAudioOutput *audioOutputSound;
    QTimer *pTimer;//1

    QTimer *my_time;
    bool canMonitor=true;//
    int value;
    int soundVolume=0;//音量大小
    int MaxValue;
    QIODevice *inputDevSound;
    QIODevice *outputDevSound;
    int useVolumeSample(short iSample);

private slots:
    void InitMonitor();
    void OnReadMore();
public slots:
    void OnSliderValueChanged(int value);
    void record_pressed();//开始录制音频
    void stop_btnPressed();//停止保存录音
    void playRecord();
    void pauseRecord();

    void select();//自动选择保存路径
};

#endif // MYTHREAD_H
