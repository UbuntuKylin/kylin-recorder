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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define KYLINRECORDER "org.kylin-recorder-data.settings"
#define FITTHEMEWINDOW "org.ukui.style"
#include <QGSettings>

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
#include <QMenu>


#include <QTimer>
#include <QTime>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDesktopWidget>
#include <QtMultimedia>//使用多媒体时要引用这个并且在pro中加QT += multimedia multimediawidgets
#include <QAudioRecorder>
#include <QListWidget>

#include <QFile>
#include <QStackedLayout>//此类提供了多页面切换的布局，一次只能看到一个界面
#include <QMouseEvent>

#include <QPainter>
#include <QSettings>
#include <QKeyEvent>
#include <QRegExp>
#include <unistd.h>
#include <fcntl.h>

//s3s4需要用DBus接口
#include <QDBusConnection>
#include <QDBusInterface>

#include "mywave.h"
#include "mythread.h"
#include "settings.h"
#include "save.h"
#include "itemswindow.h"
#include "miniwindow.h"
#include "daemondbus.h"
#include "menumodule.h"

#define INIT_MAINWINDOW_RECTANGLE_COUNT 130//用于初始化矩形条个数
class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.kylin_recorder")//调用DBus一定要加这一行
public://放在public都是有原因的因为不同类之间中调用需要公用！！
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QMessageBox *WrrMsg;
    QGSettings  *defaultPathData= nullptr;
    QGSettings *themeData=nullptr;//主题的setting

    QString limitThemeColor ;
    menuModule *menumodule = nullptr;
    ItemsWindow *itemswindow = nullptr;
    TipWindow *tipWindow = nullptr;
//    myWave *wave = nullptr;
    // 用户手册功能
    DaemonDbus *mDaemonIpcDbus;

    QString theme = "ukui-default";
    bool isMax = false;
    QTime cut;
    int valueArray[INIT_MAINWINDOW_RECTANGLE_COUNT];

    Settings set;
    Save saveas;

    //QString fileName;
    QString daultfileName;

    int ad = 1;
    int tag=0;//是否检测到输入设备

    int once=0;//没执行过

    QThread *thread;//主线程
    MyThread *myThread;//子线程
    MiniWindow mini;
    static MainWindow *mutual;//！！！指针类型静态成员变量
    bool strat_pause = false;//开始和暂停1
//    bool limitTag = true;//显示录音时间的标记
    QStackedWidget *m_pStackedWidget;//堆叠布局

    QLabel *lb;
    QToolButton *appPicture;//应用图标,标题左上角

    QListWidget *list;
    QListWidgetItem *aItem;//列表的Item
    int itemCount = 0;//文件列表的Item的个数

    QLabel *fileListlb;//文件列表
    QWidget *fileListWid;//文件列表Wid
    QHBoxLayout *fileListLayout;//文件列表布局

    QLabel *zeroFile_Messagelb;//文件列表为空时的提示信息
    QFile *file;
    void initThemeGsetting();
    void updateGsetting_ListWidget();//更新配置文件和ListWidget
    void themeButton(QString themeColor);
    void themeStyle(QString themeColor);
    void themeWindow(QString themeColor);

    void MainWindowLayout();

    QString playerTotalTime(QString filePath);

    void checkSingle();//检查单例模式
    int itemSelect=0;
    void isFileNull(int n);//检查文件列表是否为空

    int pre=0;
    int now=0;
    QTimer *pTimer;//1
    QTime baseTime;//1
    QTimer *limitTimer;//设置时间,此定时器为限制录音时长所用目前规定只允许录制不超过15分钟的录音.

    QWidget *titleRightWid;//右标题栏Wid
    QWidget *mainWid;

    bool isplaying = false;//默认文件列表播放状态为否
    bool isRecording = false;//默认没有开始录音

    QMediaPlayer *playerCompoment;
    QMediaPlaylist *playList;
    QString tempPath = "";

    QToolButton *stopButton;
    QToolButton *play_pauseButton;

private:

    int timeTag = 0;

    QList<int> maxNum;//存储振幅的大小的整型列表
    bool stop=false;//停止
    bool isFirstRun = true;

    bool max_min=false;//最大最小化

    QAudioRecorder *audioRecorder;
    QLabel *seatlb;

    QPushButton *setButton;//设置按钮
    QMenu *menu;//下拉菜单
    QAction *actionSet;//设置项
    QAction *actionHelp;//帮助项
    QAction *actionAbout;//关于项
    QList<QAction *> actions ;

    QToolButton *miniButton;//mini模式切换按钮
    QToolButton *minButton;//最小化按钮
    QToolButton *maxButton;//最大化按钮
    QToolButton *closeButton;//关闭按钮

    QPushButton *recordButton;//录音按钮
    QLabel *messageStart;//提示录音开始按钮



    QWidget *leftMainWid;//主左Wid
    QWidget *rightMainWid;//主右Wid
    QWidget *titleLeftWid;//左标题栏Wid


    //QWidget *titleWid;替换的
    QVBoxLayout *recordButtonLayout;//录制按钮布局
    QWidget *recordButtonWid;
    QWidget *listWid;//文件列表Wid


    QHBoxLayout *titleLeftLayout;//左标题栏布局
    QHBoxLayout *titleRightLayout;//右标题栏布局

    QVBoxLayout *listLayout;//文件列表布局
    //QHBoxLayout *titleLayout;//替换

    QHBoxLayout *btnLayout;
    QVBoxLayout *leftMainWidLayout;//主Wid的左布局
    QVBoxLayout *rightMainWidLayout;//主Wid的右布局
    QHBoxLayout *mainLayout;


    QHBoxLayout *showTimelbLayout;
    QHBoxLayout *monitorWaveLayout;
    QHBoxLayout *voiceAndSliderLayout;
    QHBoxLayout *controlPlay_PauseLayout;
    QVBoxLayout *ui_2Layout;

    QWidget *ui_2Wid;//第二个页面
    QWidget *showTimelbWid;
    QWidget *waveWid;//波形图的容器
    QWidget *voiceAndSliderWid;
    QWidget *controlPlay_PauseWid;

    QLabel *showTimelb;
    QFrame *fm;
    QToolButton *voiceBtn;


    //显示的时间
    QString timeStr;


    QString firstGetrecordList="";

    bool isPress;
    QPoint winPos;
    QPoint dragPos;

    // 键盘响应事件
    void keyPressEvent(QKeyEvent *event);
    //鼠标按压事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void wheelEvent(QWheelEvent *event);

    //DBus相关
    void initDbus();//初始化dbus

    bool isPlug = false;//是否是插


private://音频相关

    QSlider *slider;

    QVector<myWave*> mywave;

signals://主线程的信号

    void recordingSignal(bool);

    void startThread();
    void closeSignal();

    void startRecord();
    void stopRecord();
    void playRecord();
    void pauseRecord();
    void pageChange();




public slots:
    void getPlayingSlot(bool );//收到正在播放的信息防止文件列表再播放时录音

    void recordPaint(int); 
    void mainWindow_page2();
    void switchPage();//选择页面
    void play_pause_clicked();
    void stop_clicked();
    void updateDisplay();
    void changeVoicePicture();

    void goset();//弹出设置窗体

    void miniShow();
    void minShow();
    void maxShow();
    void closeWindow();

    void handlingSlot(bool isOk);
    void slotListItemAdd(QString fileName,int i);

//    void fileListAdd_ByStopButton(int second);

    void onPrepareForSleep(bool isSleep);//S3  S4策略
    void onPrepareForShutdown(bool Shutdown);//S3  S4策略

    void inputDevice_get(QString str);

    void limitRecordingTime();

};

#endif // MAINWINDOW_H
