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
#include "miniwindow.h"
#include "mainwindow.h"

MiniWindow::MiniWindow(QWidget *parent) : QMainWindow(parent)
{
    darkData=new QGSettings(FITTHEMEWINDOW);
    initMiniWindow();
}
void MiniWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        this->isPress = true;
        this->winPos = this->pos();
        this->dragPos = event->globalPos();
        event->accept();
    }
}

void MiniWindow::mouseReleaseEvent(QMouseEvent *event){
    this->isPress = false;
    this->setCursor(Qt::ArrowCursor);

}

void MiniWindow::mouseMoveEvent(QMouseEvent *event){
    if(this->isPress){
        this->move(this->winPos - (this->dragPos - event->globalPos()));
        this->setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}
void MiniWindow::initMiniWindow()
{
    setFixedSize(WIDTH,HEIGHT);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("麒麟录音");
    setWindowIcon(QIcon(":/svg/svg/recording_128.svg"));
    //this->setStyleSheet("border-radius:6px;");//mini窗体圆角6px
    pTimer = new QTimer;//第二个页面的控件初始化
    recordBtn=new QToolButton(this);//录制按钮
    stopBtn=new QToolButton(this);//停止按钮
    start_pauseBtn=new QToolButton(this);//开始和暂停按钮
    timelb=new QLabel(this);//显示录制时间的标签
    timelb->setText("00:00:00");
    timelb->setStyleSheet("font-size:18px;");//修改字体显示
    timelb->setFixedSize(83,15);
    max_minBtn=new QToolButton(this);//最大最小按钮
    closeBtn=new QToolButton(this);//关闭按钮

    connect(pTimer,&QTimer::timeout,this,&MiniWindow::timeDisplay);
    connect(recordBtn,&QToolButton::clicked,this,&MiniWindow::switchPage);
    connect(max_minBtn,&QToolButton::clicked,this,&MiniWindow::normalShow);
    connect(closeBtn,&QToolButton::clicked,this,&MiniWindow::closeWindow);
    connect(start_pauseBtn,&QToolButton::clicked,this,&MiniWindow::start_pauseSlot);
    connect(stopBtn,&QToolButton::clicked,this,&MiniWindow::finishSlot);


    //recordBtn->setFixedSize(24,24);
    recordBtn->setIcon(QIcon(":/png/png/recording_24.png"));
    recordBtn->setIconSize(QSize(24,24));//重置图标大小
    recordBtn->setFixedSize(24,24);
    //recordBtn->setFixedSize(24,24);
//    recordBtn->setStyleSheet("border-radius:12px;"
//                             "background-color:pink;");

    //recordBtn->setStyleSheet("QToolButton{image: url(:/png/png/recording_24.png);}");
    stopBtn->setStyleSheet("QToolButton{image: url(:/png/png/finish_mini.png);}"
                           "QToolButton:hover{image:url(:/png/png/finish_mini_hover.png);}"
                           "QToolButton:pressed{image:url(:/png/png/finish_mini_click.png);}");

    max_minBtn->setFixedSize(24,24);
    max_minBtn->setStyleSheet("QToolButton{border-radius:4px;}"
                              "QToolButton{image:url(:/png/png/max.png);}"
                              "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                              "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
    closeBtn->setFixedSize(24,24);
    closeBtn->setStyleSheet("QToolButton{border-radius:4px;}"
                            "QToolButton{image:url(:/png/png/close.png);}"
                            "QToolButton:hover{background-color:#F04234}"
                            "QToolButton:pressed{background-color:#D83436}");


    miniWid=new QWidget();//mini主窗体的Wid
    recordWid=new QWidget();//录制按钮的Wid
    pageTwoWid=new QWidget();//包括停止、开始/暂停两个按钮的Wid
    max_minAndCloseWid=new QWidget();//最大最小和关闭的Wid
    line = new QFrame();//分割线
    line->setFrameShape(QFrame::VLine);
    line->setStyleSheet("border-radius: 6px; background-color:#EFE4E1;");

    miniLayout=new QHBoxLayout();
    pageTwoLayout=new QHBoxLayout();
    max_minAndCloseLayout=new QHBoxLayout();

    pageTwoLayout->addWidget(stopBtn);
    pageTwoLayout->addWidget(start_pauseBtn);
    pageTwoWid->setLayout(pageTwoLayout);


    reWid=new QWidget();
    recordLayout=new QVBoxLayout();
    recordLayout->addWidget(recordBtn,0,Qt::AlignCenter);
    reWid->setLayout(recordLayout);

    recordStackedLayout=new QStackedLayout();//堆叠布局
    recordStackedLayout->addWidget(reWid);
    recordStackedLayout->addWidget(pageTwoWid);
    recordWid->setLayout(recordStackedLayout);


    max_minAndCloseLayout->addWidget(line);
    max_minAndCloseLayout->addWidget(max_minBtn);
    max_minAndCloseLayout->addWidget(closeBtn);
    max_minAndCloseLayout->setSpacing(0);
    max_minAndCloseLayout->setMargin(0);
    max_minAndCloseWid->setLayout(max_minAndCloseLayout);

    miniLayout->addWidget(recordWid,0,Qt::AlignCenter);
    miniLayout->addWidget(timelb);
    miniLayout->addWidget(max_minAndCloseWid);
    miniLayout->setSpacing(0);
    miniLayout->setMargin(0);
    miniWid->setLayout(miniLayout);
    setCentralWidget(miniWid);

    if(darkData->get("style-name").toString() == "ukui-dark"||darkData->get("style-name").toString() == "ukui-black")
    {
//        start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
//                                           "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
//                                           "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");


//        miniWid->setStyleSheet("border-radius:6px;background-color:#222222;");//后期适配主题颜s;
    }
    else
    {
//        //初始按钮为暂停按钮。因为只要点击录音按钮就要开始录制了
//        start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
//                                 "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
//                                 "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");
//        miniWid->setStyleSheet("border-radius:6px;background-color:#FBEFED;");//自定义窗体(圆角+背景色)

    }



    setAttribute(Qt::WA_TranslucentBackground);//主窗体透明
    move((QApplication::desktop()->width() - WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);

}

void MiniWindow::normalShow()
{
    MainWindow::mutual->showNormal();
    this->hide();
}
void MiniWindow::closeWindow()//关闭mini和主窗体
{
     this->close();
     MainWindow::mutual->thread->quit();
     MainWindow::mutual->thread->wait();
     MainWindow::mutual->close();

}
void MiniWindow::start_pauseSlot()
{
    static QTime pauseTime;
    if(start_pause)
    {
        if(darkData->get("style-name").toString() == "ukui-dark"||darkData->get("style-name").toString() == "ukui-black")
        {
            start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
                                               "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");

        }
        else
        {
            start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                           "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                           "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");
        }

        MainWindow::mutual->strat_pause=true;//区分迷你模式中的start_pause
        start_pause=false;
        MainWindow::mutual->play_pause_clicked();//开始录制
        QTime cut = QTime::currentTime();//记录开始时的时间
        //qDebug()<<cut;
        int t = pauseTime.secsTo(cut);//点击暂停时时间与点击恢复计时的时间差值
        qDebug()<<t;
        baseTime = baseTime.addSecs(t);
        //qDebug()<<baseTime;
        pTimer->start(1);

    }
    else
    {
        MainWindow::mutual->strat_pause=false;//区分迷你模式中的start_pause
        start_pause=true;
        MainWindow::mutual->play_pause_clicked();//暂停录制
        pTimer->stop();
        //MainWindow::mutual->pTimer->stop();
        pauseTime = QTime::currentTime();//记录一下当前暂停时的时间
        qDebug()<<pauseTime;
        if(darkData->get("style-name").toString() == "ukui-dark"||darkData->get("style-name").toString() == "ukui-black")
        {
            start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini_white.png);}"
                                               "QToolButton:hover{image:url(:/png/png/start_mini_hoverWhite.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/start_mini_clickWhite2.png);}");
        }
        else
        {
            start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini.png);}"
                           "QToolButton:hover{image:url(:/png/png/start_mini_hover.png);}"
                           "QToolButton:pressed{image:url(:/png/png/start_mini_click.png);}");
        }




    }

}
void MiniWindow::finishSlot()//结束录音，并保存文件
{
    qDebug()<<"录音结束！";
    pTimer->stop();//计时停止
    MainWindow::mutual->stop_clicked();//主线程停止录音
    //mythr.stop_btnPressed();
    if(start_pause)
    {
        //记得恢复初始状态
        start_pause = false;
        MainWindow::mutual->strat_pause=false;
    }
    timelb->setText("00:00:00");
    recordStackedLayout->setCurrentIndex(0);//mini模式切换至录音按钮
    MainWindow::mutual->m_pStackedLayout->setCurrentIndex(0);//主界面切换至录音按钮
}
void MiniWindow::timeDisplay()
{
    QTime currTime = QTime::currentTime();
    //qDebug()<<currTime;
    int t = MainWindow::mutual->baseTime.secsTo(currTime);
    //qDebug()<<baseTime;
    QTime showTime(0,0,0);
    showTime = showTime.addSecs(t);
    //qDebug()<<t;
    this->timeStr = showTime.toString("hh:mm:ss");
//    qDebug()<<"*********"<<timeStr;
    this->timelb->setText(timeStr);
}
void MiniWindow::switchPage()//换页
{
    baseTime = baseTime.currentTime();
    pTimer->start(1);
    mythr.soundVolume = 50;//初始录音要有音量大小，否则默认音量为0，检测的声音就是0了
    if(darkData->get("style-name").toString() == "ukui-dark"||darkData->get("style-name").toString() == "ukui-black")
    {
        start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
                                      "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
                                      "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");
    }
    else
    {
        start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                       "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                       "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");
    }

    MainWindow::mutual->switchPage();//主线程开始录音
    MainWindow::mutual->m_pStackedLayout->setCurrentIndex(1);//mini模式切换至两个按钮页面
    recordStackedLayout->setCurrentIndex(1);//主界面切换至两个按钮页面
}
