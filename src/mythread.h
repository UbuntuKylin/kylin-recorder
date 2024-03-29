/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: baijincheng <baijincheng@kylinos.cn>
 */
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
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QAudio>
#include <QGSettings>

#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QAudioRecorder>
#include <QAudioProbe>
#include <QFile>
#include <QIODevice>
//文件监视器
#include <QFileSystemWatcher>

#include "settings.h"
#include "save.h"
#include "mp3encoder.h"
#include "itemswindow.h"
#include "tipwindow.h"
#include "ffutil.h"
//==================

#define BufferSize 35280
#define MYTHREAD_RECTANGLE_COUNT 130//检测时矩形条个数实时更新
class MyThread : public QWidget
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
    int soundVolume=65;//初始音量为70
    Settings set;
    Save saveas;
    QString fileName;
    QString daultfileName;
    QString default_Location;

    QGSettings  *recordData= nullptr;
    int i=0;

    bool isRecordStart = false;

    QString readPathCollected();
    int readNumList();
    void writePathCollected(QString filePath);//更新配置文件
    void writeNumList(int num);
    void onChangeCurrentRecordList(QString filePath);
    QString endFileName;
    QString isSameFileName(QString endFileName);

    void updateAmplitudeList(int valueArray[]);//更新振幅列表
    void saveAs(QString oldFileName);//右键另存为
    void OpenFolderProcess(QString openedPath);//打开文件夹的命令


    QString absolutionPath;//raw存放的绝对路径
    QString outputFileName;//输出的文件名
    QString recordTime;

    QTimer *tipTimer;//延时执行process

    QAudioRecorder *audioRecorder;
    QAudioProbe *audioProbe;

    int x = 0;

signals:
    void recordPaint(int);
    void stopRecord();
    void changeVoicePicture();
    void listItemAddSignal(QString fileName,QString recordTime);//更新检测到的音频振幅值到配置文件
    void handling(bool isOk);

private:

    int beishu=1;//倍数
    int quzhi=0;//取值
    int xianzhi=0;//限制为110个
    //int valueArray[110];//主队列
    QList<int> tmpArray2;//副队列
    qint64 toConvertWAV(QString catheFileName , QString wavFileName);
    qint64 toConvertMp3(QString catheFileName , QString mp3FileName);
    qint64 toConvertM4a(QString catheFileName , QString m4aFileName);
    QAudioFormat Mp3();
    QAudioFormat Wav();
    QAudioFormat M4a();


    QFile *file=nullptr;
    QAudioInput * audioInputFile =nullptr;//1将音频输入文件
    QAudioOutput* audioOutputFile = nullptr;//1
    short *outdata=nullptr;



    qint64 len;
    QAudioFormat mFormatSound;
    QAudioInput *audioInputSound=nullptr;		// 负责监听声音
    QAudioOutput *audioOutputSound=nullptr;


    //mp3

    QAudioInput *ainput=nullptr;
    QIODevice *dev=nullptr;
    Mp3Encoder *encoder=nullptr;
    char *mp3_data;
    QFile *fl=nullptr;

    QTimer *my_time;
    bool canMonitor=true;//
    int value;

    int MaxValue;
    QIODevice *inputDevSound;
    QIODevice *outputDevSound;
    int useVolumeSample(qreal iSample);
    int NUM=0;

    QAudioDeviceInfo monitorVoiceSource(int i);

    QString setDefaultPath(QString path);//设置默认存储路径解决中英文路径冲突问题

    QString pathSetting();


private slots:
    void InitMonitor();
    void OnReadMore();

    void uploadStarted();//加载tips界面

    void updateProgress(qint64 duration);//更新时间进度
public slots:




    void OnSliderValueChanged(int value);
    void record_pressed();//开始录制音频
    void stop_btnPressed();//停止保存录音
    void playRecord();
    void pauseRecord();


    void audioConversionFinish(int isOk,QProcess::ExitStatus);



    void processBuffer(const QAudioBuffer &buffer);

private:


    QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);
    qreal getPeakValue(const QAudioFormat& format);
    template <class T>
    QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

};

#endif // MYTHREAD_H
