#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QPainter>
#include <QStyle>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QThread>


#include <QTimer>
#include <QTime>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDesktopWidget>
#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QAudioRecorder>
#include <QFile>
#include <QStackedLayout>//此类提供了多页面切换的布局，一次只能看到一个界面

#include <QPainter>
#include <QSettings>
#include "mywave.h"
#include "mythread.h"
#include "settings.h"
#include "save.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Settings set;
      Save saveas;
      //QString fileName;
      QString daultfileName;
      QString desktop_path;
      int ad = 1;
private:
    QThread *thread;//主线程
    MyThread *myThread;//子线程

    QList<int> maxNum;//存储振幅的大小的整型列表
    bool stop=false;//停止
    bool strat_pause=false;//开始和暂停1
    bool max_min=false;//最大最小化

    QAudioRecorder *audioRecorder;
    QProgressBar *process;
    QToolButton *setButton;
    QToolButton *max_minButton;//最大最小化切换按钮

    QToolButton *closeButton;
    QToolButton *recordButton;

    QToolButton *stopButton;
    QToolButton *play_pauseButton;
    QWidget *mainWid;
    QWidget *titleWid;
    QWidget *recordButtonWid;

    QHBoxLayout *titleLayout;
    QHBoxLayout *btnLayout;
    QVBoxLayout *mainLayout;

    QStackedLayout *m_pStackedLayout;//堆叠布局
    QHBoxLayout *layout1;
    QHBoxLayout *layout2;
    QHBoxLayout *layout3;
    QHBoxLayout *layout4;
    QVBoxLayout *ui_2Layout;

    QWidget *ui_2Wid;//第二个页面
    QWidget *timeWid;
    QWidget *waveWid;//波形图的容器
    QWidget *voiceWid;
    QWidget *playWid;

    QLabel *showTimelb;
    QFrame *fm;
    QLabel *voicelb;

    QTimer *pTimer;//1
    QTime baseTime;//1
    QTimer *my_time;
    //显示的时间
    QString timeStr;

signals://主线程的信号
    void startThread();
    void closeSignal();

    void startRecord();
    void stopRecord();
    void playRecord();
    void pauseRecord();

private://音频相关

    QSlider *slider;

    QVector<myWave*> mywave;
    QTimer *updateTimer31;



public slots:
    void recordPaint(int);
    void max_minShow();
    void mainWindow_page2();
    void switchPage();
    void play_pause_clicked();
    void stop_clicked();
    void updateDisplay();
    void changeVoicePicture();

    void setuserdirectory();
    void goset();//弹出设置窗体

};

#endif // MAINWINDOW_H
