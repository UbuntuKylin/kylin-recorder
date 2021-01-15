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
#include "daemondbus.h"

class MiniWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MiniWindow(QWidget *parent = nullptr);
    int WIDTH=220;
    int HEIGHT=36;
    QGSettings *darkData=nullptr;//主题的setting

    MyThread mythr;//mini模式中用于调用录音子线程
    QStackedWidget*recordStackedWidget;//堆叠布局

    QToolButton *start_pauseBtn;//mini模式的开始和暂停按钮
    bool start_pause=false;//mini模式的判断开始或暂停是否按下
    bool stop = false;

    QTimer *pTimer;//1
    QTime baseTime;//1
    //显示的时间
    QString timeStr;//时间串
    QLabel *timelb;//显示录制时间的标签
    QWidget *miniWid;//mini主窗体的Wid
    QFrame *line;
    QToolButton *closeBtn;//关闭按钮
    QToolButton *recordBtn;//录制按钮
    QToolButton *stopBtn;//停止按钮
private:

    QVBoxLayout *recordLayout;
    QWidget *reWid;
    QWidget *recordWid;//录制按钮的Wid
    QWidget *pageTwoWid;//包括停止、开始/暂停两个按钮的Wid
    QWidget *max_minAndCloseWid;//最大最小和关闭的Wid

    QHBoxLayout *miniLayout;
    QHBoxLayout *pageTwoLayout;
    QHBoxLayout *max_minAndCloseLayout;







    QToolButton *miniBtn;//最大最小按钮



private:

    bool isPress;
    QPoint winPos;
    QPoint dragPos;

    // 用户手册功能
    DaemonDbus *mDaemonIpcDbus;
    // 键盘响应事件
    void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void initMiniWindow();

    bool eventFilter(QObject *obj, QEvent *event);//F1事件过滤

public slots:
    void switchPage();
    void normalShow();
    void closeWindow();
    void start_pauseSlot();
    void finishSlot();//结束录音并保存
    void timeDisplay();
signals:

};

#endif // MINIWINDOW_H
