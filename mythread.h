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
#include <QGSettings>

#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QAudioRecorder>
#include <QFile>
#include <QIODevice>
#include "settings.h"
#include "save.h"
#include "mp3encoder.h"
#include "itemswindow.h"
//==================

#define BufferSize     35280
//#define cutRectangleCount 109//矩形条个数
class MyThread : public QMainWindow
{
    Q_OBJECT
public:
    explicit MyThread(QWidget *parent = nullptr);

    QAudioFormat format;

    int isSuccess = 1;//默认音频转码1不成功，
    QProcess *process;
    int count=0;

    int type=1;
    QMessageBox *WrrMsg;
    int soundVolume=0;//音量大小
    Settings set;
    Save saveas;
    QString fileName;
    QString daultfileName;
    QString desktop_path;

    QGSettings  *recordData= nullptr;
    int i=0;

    QString readPathCollected();
    int readNumList();
    void writePathCollected(QString filePath);//更新配置文件
    void writeNumList(int num);
    void onChangeCurrentRecordList(QString filePath);

    void updateAmplitudeList(int valueArray[]);//更新振幅列表
    void test();

signals:
    void recordPaint(int);
    void stopRecord();
    void changeVoicePicture();
    void listItemAddSignal(QString fileName,int i);//更新检测到的音频振幅值到配置文件
    void handling(QString str);
private:
    int beishu=1;//倍数
    int quzhi=0;//取值
    int xianzhi=0;//限制为110个
    //int valueArray[110];//主队列
    QList<int> tmpArray2;//副队列
    qint64 addWavHeader(QString catheFileName , QString wavFileName);
    qint64 toConvertMp3(QString catheFileName , QString mp3FileName);
    qint64 toConvertM4a(QString catheFileName , QString m4aFileName);
    QAudioFormat Mp3();
    QAudioFormat Wav();


    QFile *file;
    QAudioInput * audioInputFile;//1将音频输入文件
    QAudioOutput* audioOutputFile;//1
    short *outdata;



    qint64 len;
    QAudioFormat mFormatSound;
    QAudioInput *audioInputSound;		// 负责监听声音
    QAudioOutput *audioOutputSound;
    QTimer *pTimer;//1

    //mp3

    QAudioInput *ainput;
    QIODevice *dev;
    Mp3Encoder *encoder;
    char *mp3_data;
    QFile *fl;

    QTimer *my_time;
    bool canMonitor=true;//
    int value;

    int MaxValue;
    QIODevice *inputDevSound;
    QIODevice *outputDevSound;
    int useVolumeSample(short iSample);
    int NUM=0;
    void listItemAdd(QString fileName);

    QAudioDeviceInfo monitorVoiceSource(int i);


private slots:
    void InitMonitor();
    void OnReadMore();

    //void sendCurrentRecordList(QString filePath);
public slots:
    void OnSliderValueChanged(int value);
    void record_pressed();//开始录制音频
    void stop_btnPressed();//停止保存录音
    void playRecord();
    void pauseRecord();

    void selectMp3();//选择保存路径Mp3
    void selectWav();//选择保存路径Wav
    void selectM4a();//选择保存路径M4a
    void audioConversionFinish(int isOk);

};

#endif // MYTHREAD_H
