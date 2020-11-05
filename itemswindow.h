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
#include "mythread.h"
#include <QDebug>
class ItemsWindow : public QMainWindow
{
    Q_OBJECT
public:
    QWidget *itemsWid;
    QWidget *mainWid;
    QHBoxLayout *mainLayout;
    explicit ItemsWindow(QWidget *parent = nullptr);
    QLabel *recordFileNamelb;//录音文件名
    QLabel *dateTimelb;//日期时间
    QWidget *stackWid;
    QWidget *firstWid;
    QWidget *threeButtonWid;//三个按钮的Wid

    QWidget *clipperWid;//剪裁的总Wid
    QWidget *clipperstackWid;//
    QFrame *line;//剪裁线独自占一个
    QWidget *bottomWid;//底部Wid

    QLabel *timelengthlb2;//音频时间
    QToolButton *cancelButton;//取消
    QToolButton *finishButton;//完成
    QHBoxLayout *bottomLayout;//底部布局
    QVBoxLayout *clipperLayout;//剪裁布局
    QStackedLayout *clipperstackLayout;//剪裁堆叠布局


    QLabel *timelengthlb;//测试label
    QTime totalTime;
//    void enterEvent(QEvent*);
//    void leaveEvent(QEvent*);
    QMessageBox *WrrMsg;
    QLabel *testlb;

    QMediaPlaylist *playList;
    QMediaPlayer *player;

    QStackedLayout *stackLayout;

private:
    bool play_pause=false;
    QLabel *fileListlb;//文件列表
    QSlider *playSlider;//播放划块条



    QHBoxLayout *timeLengthLayout;

    QToolButton *itemPlay_PauseButton;//列表项的播放与暂停按钮
    QToolButton *clipButton;//裁剪按钮
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
    void setItemWid();
//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *e);
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);

    //当前播放的录音索引
    int currentPlayIndex = -1;
private slots:

    void itemPlay_PauseClicked();
    void positionChange(qint64 position);
    void durationChange(qint64 duration);
    void setPosition(int position);
    bool eventFilter(QObject *obj, QEvent *event);
    // 拖动进度条
    void slidePress();
    //滑动条弹起
    void slideRelease();

    //裁剪
    void clipper();
    //删除
    void delFile();
public slots:


signals:
    void judgePlay(int currentIndex);

};

#endif // ITEMSWINDOW_H
