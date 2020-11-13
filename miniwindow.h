#ifndef MINIWINDOW_H
#define MINIWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QToolButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QFrame>
#include <QTimer>
#include <QTime>

#include "mythread.h"

class MiniWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MiniWindow(QWidget *parent = nullptr);
    int WIDTH=220;
    int HEIGHT=36;
    QGSettings *darkData=nullptr;//主题的setting
    MyThread mythr;//mini模式中用于调用录音子线程
    QStackedLayout *recordStackedLayout;//堆叠布局

    QToolButton *start_pauseBtn;//mini模式的开始和暂停按钮
    bool start_pause=false;//mini模式的判断开始或暂停是否按下

    QTimer *pTimer;//1
    QTime baseTime;//1
    //显示的时间
    QString timeStr;//时间串
    QLabel *timelb;//显示录制时间的标签
    QWidget *miniWid;//mini主窗体的Wid
    QFrame *line;
private:

    QVBoxLayout *recordLayout;
    QWidget *reWid;
    QWidget *recordWid;//录制按钮的Wid
    QWidget *pageTwoWid;//包括停止、开始/暂停两个按钮的Wid
    QWidget *max_minAndCloseWid;//最大最小和关闭的Wid

    QHBoxLayout *miniLayout;
    QHBoxLayout *pageTwoLayout;
    QHBoxLayout *max_minAndCloseLayout;


    QToolButton *recordBtn;//录制按钮
    QToolButton *stopBtn;//停止按钮



    QToolButton *max_minBtn;//最大最小按钮
    QToolButton *closeBtn;//关闭按钮


private:

    bool isPress;
    QPoint winPos;
    QPoint dragPos;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void initMiniWindow();


public slots:
    void switchPage();
    void maxShow();
    void closeWindow();
    void start_pauseSlot();
    void finishSlot();//结束录音并保存
    void timeDisplay();
signals:

};

#endif // MINIWINDOW_H
