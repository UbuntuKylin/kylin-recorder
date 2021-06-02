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
 *  Authors: baijincheng <baijincheng@kylinos.cn>
 */
#ifndef ITEMSWINDOW_H
#define ITEMSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QTime>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QGSettings>
#include <QMenu>
#include <QAction>

#include <QFile>
#include <QDesktopServices>
#include <QProcess>
#include <QTextCodec>

#include "mythread.h"
#include "mywave.h"
#include "clipbutton.h"
#include <QDebug>

#define ITEMWINDOW_RECTANGLE_COUNT 130//用于绘制剪裁界面的矩形条个数
class ItemsWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit ItemsWindow(QWidget *parent = nullptr);
    ~ItemsWindow();
    int initTag=0;
    int isPlayerEnd = 0;//是当前音频文件是否播放结束
    QGSettings  *itemData= nullptr;
    QGSettings *darkData=nullptr;//主题的setting
    QWidget *itemsWid;
    QWidget *mainWid;
    QHBoxLayout *mainLayout;

    QString executeLinuxCmd(QString);
    QString strResult1;
    myWave *wave = nullptr;


    QString itemsThemeColor;//主题颜色
    QLabel *listNum;//录音序号
    QLabel *recordFileName;//录音文件名
    QLabel *listNumChangeColor;//录音序号颜色变化
    QLabel *recordFileNameChangeColor;//录音文件名颜色变化
    QWidget *stackWid;
    QWidget *firstWid;
    QWidget *threeButtonWid;//三个按钮的Wid

    QWidget *clipperWid;//剪裁的总Wid
    QStackedWidget *clipperstackWid;//用于切换剪裁的堆叠wid
    QStackedWidget *splitLinestackWid;//分割线堆叠wid
    QFrame *line;//分割线
    ClipButton *cursor;//游标
    QWidget *cutWaveWid;//剪裁界面框(滑块+轨道)
    QWidget *railWid;//轨道的Wid
    QWidget *bottomWid;//底部Wid
    QString clipFilePath;//被剪裁的文件路径

    QLabel *timelengthlb2;//音频时间节点(开始/结束)
//    QLabel *liveTime;//音频时间实时显示
    int time;
    QTimer *moveTime;//每隔毫秒数
    int movePos = 0;//每隔毫秒数所移动位置
    int cursorCanMove = 1;//初始默认游标可移动

    QToolButton *cancelButton;//取消
    QToolButton *finishButton;//完成
    QProcess *process;
    QString timeEditStartTime;
    QString timeEditEndTime;
    int start_Time = 0;//剪辑开始节点,默认都是0
    int end_Time = 0;//剪辑结束节点,默认都是0

    QHBoxLayout *bottomLayout;//底部布局
    QVBoxLayout *clipperLayout;//剪裁布局
    QHBoxLayout *waveLayout;//波形图布局
    QHBoxLayout *rectangleLayout;
//    QStackedLayout *clipperstackLayout;//剪裁堆叠布局
    QList<int> amplitudeNum;//存储振幅的大小的整型列表
    QLabel *timelengthlb;//测试label
    QTime totalTime;
//    void enterEvent(QEvent*);
//    void leaveEvent(QEvent*);
    QMessageBox *WrrMsg;
    QLabel *testlb;

    QString audioFilePath;//可用路径
    QString isDeletePath;//已被删除的标记路径

    QString tempPath= "";
    bool isOpen = false;

    QStackedLayout *stackLayout;

    QMenu *menu;
    QAction *actionSave;//另存为
    QAction *actionOpenFolder;//打开此音频所在文件夹

    bool play_pause=false;

    bool stop = false;
    bool pause = false;

    int createCutWave();
    ClipButton *leftBtn;//左箭头按钮
    ClipButton *rightBtn;//右箭头按钮

     void judgeState(enum QMediaPlayer::State,QString path);//判断播放状态
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    //bool isplay=false;
private:

    QVector<myWave*> mywave;


    QLabel *fileListlb;//文件列表
    QSlider *playSlider;//播放划块条



    QHBoxLayout *timeLengthLayout;

    QToolButton *itemPlay_PauseButton;//列表项的播放与暂停按钮
    QToolButton *clipButton;//进入裁剪界面的按钮

    QToolButton *deleteButton;//删除按钮
    QWidget *itemWid;//列表项Wid

    QListWidgetItem *aItem;//列表的Item
    int itemNum=0;

    QWidget *itemTopWid;//列表项的顶部Wid，只有一个划块
    QWidget *itemBottomWid;//列表项的底部Wid，有label和button两组控件
    QWidget *fileName_dateTimeWid;//文件名和日期时间的Wid

    QWidget *recordAndlistWid;//录音图标和文件列表的Wid

    QVBoxLayout *itemLayout;//列表项布局

    QHBoxLayout *itemTopLayout;//列表项的顶部布局
    QHBoxLayout *itemBottomLayout;//列表项的底部布局
    QVBoxLayout *fileName_dateTimeLayout;//列表项的label布局
    QHBoxLayout *threeButtonLayout;//列表项的3个按钮布局


private:

    static void _processStart(const QString &cmd , QStringList arguments = QStringList());
    static void deleteImage(const QString &savepath);
    void initRectangleWave();//停止录音后再生成110个矩形框,防止多次初始化造成cpu占用率过高
    void deleteWaves();

    void initItemWid();
    void setItemWid();
    void initClipper();
    void setClipper();
    void initThemeGsetting();//配置文件初始化主题
    void themeStyle(QString themeColor);

    void stopReplayer();

    void playState();
    void isOtherClipWidOpen();

    void clipperFun();


    void listFileNumUpdate(int num);

    void hover_ChangeState(QEvent *event);
    QPoint pressPoint;

    void delUpdateGSetting(QString fileName);

signals:

    void playingSignal(bool);//发送正在播放的信号，通知播放时不可以录音

    void updateGSettingSignal(QString fileName);
public slots:
    void updateGSettingSlot(QString fileName);
private slots:

    void itemPlay_PauseClicked();
    void positionChange(qint64 position);
    void durationChange(qint64 duration);
    void stateChanged(enum QMediaPlayer::State);
    void setPosition(int position);


    void rightClickedMenuRequest();//右击弹出Menu菜单选择另存为和打开文件位置
    void actionSaveasSlot();
    void actionOpenFolderSlot();

    // 拖动进度条
    void slidePress();
    //滑动条弹起
    void slideRelease();

    //裁剪
    void clipper();
    void processFinish(int);
    //删除本地音频文件
    void delFile();

    void cancel();
    void finish();

    void pressPausePlayer_Slot();
    void leftBtn_ReleaseStartPlayer_Slot(int leftButton_absolutePos,int leftButton_rightBorderOppositive,int padding);//左按钮或右按钮,统一为按钮边界相对位置
    void rightBtn_ReleaseGetEndPositon_Slot(int rightButton_absolutePos,int rightButton_leftBorderOppositive,int padding);

    void cursorMove();


    void leftButton_rightBorderSlot(int x);

public :
    bool isRecording = false;//是否正在录音

public slots:
    void getRecordingSlot(bool);


};

#endif // ITEMSWINDOW_H
