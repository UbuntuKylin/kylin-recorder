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

#include "mythread.h"
#include "mywave.h"
#include <QDebug>


#define cutRectangleCount 110//剪裁界面的矩形条个数
class ItemsWindow : public QMainWindow
{
    Q_OBJECT
public:

    int initTag=0;

    QGSettings  *itemData= nullptr;
    QGSettings *darkData=nullptr;//主题的setting
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
    QStackedWidget *clipperstackWid;//用于切换剪裁的堆叠wid
    QStackedWidget *splitLinestackWid;//分割线堆叠wid
    QFrame *line;
//    QFrame *line;
    QWidget *cutWaveWid;//剪裁线独自占一个
    QWidget *bottomWid;//底部Wid

    QLabel *timelengthlb2;//音频时间
    QToolButton *cancelButton;//取消
    QToolButton *finishButton;//完成
    QHBoxLayout *bottomLayout;//底部布局
    QVBoxLayout *clipperLayout;//剪裁布局
    QHBoxLayout *waveLayout;//波形图布局
//    QStackedLayout *clipperstackLayout;//剪裁堆叠布局
    QList<int> amplitudeNum;//存储振幅的大小的整型列表

    QLabel *timelengthlb;//测试label
    QTime totalTime;
//    void enterEvent(QEvent*);
//    void leaveEvent(QEvent*);
    QMessageBox *WrrMsg;
    QLabel *testlb;

    QMediaPlaylist *playList;
    QMediaPlayer *player;

    QStackedLayout *stackLayout;

    QMenu *menu;
    QAction *actionSave;//另存为
    QAction *actionOpenFolder;//打开此音频所在文件夹

    bool play_pause=false;


    void createCutWave();


    //bool isplay=false;
private:

    QVector<myWave*> mywave;


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

    void initRectangleWave();//停止录音后再生成110个矩形框,防止多次初始化造成cpu占用率过高
    void deleteWaves();

    void initItemWid();
    void setItemWid();
    void initClipper();
    void setClipper();
    void stopReplayer();
    void playState();

    void rightClickedMenuRequest();//右击弹出Menu菜单选择另存为和打开文件位置

//    void mouseMoveEvent(QMouseEvent *event);
//    void mousePressEvent(QMouseEvent *e);
//    void enterEvent(QEvent *event);
//    void leaveEvent(QEvent *event);

private slots:

    void itemPlay_PauseClicked();
    void positionChange(qint64 position);
    void durationChange(qint64 duration);
    void setPosition(int position);
    bool eventFilter(QObject *obj, QEvent *event);


    void actionSaveasSlot();
    void actionOpenFolderSlot();

    // 拖动进度条
    void slidePress();
    //滑动条弹起
    void slideRelease();

    //裁剪
    void clipper();
    //删除
    void delFile();

    void cancel();
    void finish();
public slots:


};

#endif // ITEMSWINDOW_H
