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
#include "miniwindow.h"
#include "mainwindow.h"
#include "xatom-helper.h"

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
    // 用户手册功能
    mDaemonIpcDbus = new DaemonDbus();
    miniWid=new QWidget();//mini主窗体的Wid

    // 添加窗管协议
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(miniWid->winId(), hints);

    miniWid->setFixedSize(WIDTH,HEIGHT);
    QScreen *screen = QGuiApplication::primaryScreen();
    miniWid ->move((screen->geometry().width() - WIDTH) / 2,(screen->geometry().height() - HEIGHT) / 2);

    miniWid->installEventFilter(this);
    miniLayout=new QHBoxLayout(miniWid);
    recordWid=new QWidget();//录制按钮的Wid
    pageTwoWid=new QWidget();//包括停止、开始/暂停两个按钮的Wid
    max_minAndCloseWid=new QWidget();//最大最小和关闭的Wid

    pageTwoLayout = new QHBoxLayout();
    max_minAndCloseLayout = new QHBoxLayout();
    reWid = new QWidget();
    recordLayout = new QVBoxLayout();
    line = new QFrame(miniWid);//分割线



//    setWindowFlags(Qt::FramelessWindowHint);
    miniWid -> setWindowTitle(tr("kylin-recorder"));
    miniWid -> setWindowIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/svg/svg/recording_128.svg")));
    //this->setStyleSheet("border-radius:6px;");//mini窗体圆角6px
    pTimer = new QTimer;//第二个页面的控件初始化
    recordBtn=new QToolButton(this);//录制按钮
    recordBtn->setToolTip(tr("Recording"));
    stopBtn=new QToolButton(pageTwoWid);//停止按钮
    stopBtn->setToolTip(tr("Stop"));
    start_pauseBtn=new QToolButton(pageTwoWid);//开始和暂停按钮
    start_pauseBtn->setToolTip(tr("Start/Pause"));

    timelb=new QLabel(this);//显示录制时间的标签
    timelb->setText("00:00:00");
    timelb->setStyleSheet("font-size:18px;");//修改字体显示
    timelb->setFixedSize(83,15);
    miniBtn=new QToolButton(this);//复原按钮
    miniBtn->setIcon(QIcon::fromTheme("window-restore-symbolic"));//主题库的mini图标
    miniBtn->setFixedSize(24,24);
    miniBtn->setProperty("isWindowButton", 0x1);
    miniBtn->setProperty("useIconHighlightEffect", 0x2);
    miniBtn->setAutoRaise(true);
    miniBtn->setToolTip(tr("Restore"));

    closeBtn=new QToolButton(this);//关闭按钮
    closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));//主题库的叉子图标
    closeBtn->setFixedSize(24,24);
    closeBtn->setProperty("isWindowButton", 0x2);
    closeBtn->setProperty("useIconHighlightEffect", 0x8);
    closeBtn->setAutoRaise(true);
    closeBtn->setToolTip(tr("Close"));

    connect(pTimer,&QTimer::timeout,this,&MiniWindow::timeDisplay);
    connect(recordBtn,&QToolButton::clicked,this,&MiniWindow::switchPage);
    connect(miniBtn,&QToolButton::clicked,this,&MiniWindow::normalShow);
    connect(closeBtn,&QToolButton::clicked,this,&MiniWindow::closeWindow);
    connect(start_pauseBtn,&QToolButton::clicked,this,&MiniWindow::start_pauseSlot);
    connect(stopBtn,&QToolButton::clicked,this,&MiniWindow::finishSlot);


    //recordBtn->setFixedSize(24,24);
    recordBtn->setIcon(QIcon(":/svg/svg/mini-recording.svg"));
    recordBtn->setFixedSize(24,24);
    recordBtn->setIconSize(QSize(24,24));//重置图标大小

    stopBtn->setStyleSheet("QToolButton{border-radius:12px;image: url(:/svg/svg/finish_mini.svg);}"
                           "QToolButton:hover{image:url(:/svg/svg/finish_mini_hover.svg);}"
                           "QToolButton:pressed{image:url(:/svg/svg/finish_mini_click.svg);}");

    line->setFixedWidth(1);
    line->setFrameShape(QFrame::VLine);





    pageTwoLayout->addWidget(stopBtn);
    pageTwoLayout->addWidget(start_pauseBtn);
    pageTwoWid->setLayout(pageTwoLayout);


    recordLayout->addWidget(recordBtn,0,Qt::AlignCenter);
    recordLayout->setContentsMargins(5,5,5,5);//左上右下
    reWid->setLayout(recordLayout);

    recordStackedWidget = new QStackedWidget();//录音标签和开始暂停按钮的堆叠布局
    recordStackedWidget->addWidget(reWid);
    recordStackedWidget->addWidget(pageTwoWid);


    max_minAndCloseLayout->addWidget(line);
    max_minAndCloseLayout->addWidget(miniBtn);
    max_minAndCloseLayout->addWidget(closeBtn);
    max_minAndCloseLayout->setSpacing(0);
    max_minAndCloseLayout->setMargin(0);
    max_minAndCloseWid->setLayout(max_minAndCloseLayout);

    miniLayout->addWidget(recordStackedWidget,0,Qt::AlignCenter);
    miniLayout->addWidget(timelb);
    miniLayout->addWidget(max_minAndCloseWid);
    miniLayout->setSpacing(0);
    miniLayout->setMargin(0);
//    miniWid->setLayout(miniLayout);
//    setCentralWidget(miniWid);

//    setAttribute(Qt::WA_TranslucentBackground);//主窗体透明

//    this->move((QApplication::desktop()->width() - WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);
//    miniWid->show();
}

// 实现键盘响应
void MiniWindow::keyPressEvent(QKeyEvent *event)
{
    // F1快捷键打开用户手册
    if (event->key() == Qt::Key_F1) {
        if (!mDaemonIpcDbus->daemonIsNotRunning()){
            //F1快捷键打开用户手册，如kylin-recorder
            //由于是小工具类，下面的showGuide参数要填写"tools/kylin-recorder"
            mDaemonIpcDbus->showGuide("tools/kylin-recorder");
        }
    }
}

bool MiniWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(obj == miniWid)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_F1)  //将event转换为QKeyEvent,然后判断是否键
        {
            qDebug()<<"按压了F1";
            if (!mDaemonIpcDbus->daemonIsNotRunning()){
                //F1快捷键打开用户手册，如kylin-recorder
                //由于是小工具类，下面的showGuide参数要填写"tools/kylin-recorder"
                mDaemonIpcDbus->showGuide("tools/kylin-recorder");
            }
            return true;
        }

    }
    return QObject::eventFilter(obj,event);
}
void MiniWindow::normalShow()
{
    miniWid->hide();
    MainWindow::mutual->mainWid->showNormal();

}
void MiniWindow::closeWindow()//关闭mini和主窗体
{
     MainWindow::mutual->closeWindow();
     miniWid->close();
     MainWindow::mutual->thread->quit();
     MainWindow::mutual->thread->wait();
     MainWindow::mutual->close();

}
void MiniWindow::start_pauseSlot()
{
    static QTime pauseTime;
    if(start_pause)
    {
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
    recordStackedWidget->setCurrentIndex(0);//mini模式切换至录音按钮
    MainWindow::mutual->m_pStackedWidget->setCurrentIndex(0);//主界面切换至录音按钮
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
    if(!MainWindow::mutual->isplaying)
    {
        baseTime = baseTime.currentTime();
        pTimer->start(1);
        mythr.soundVolume = 50;//初始录音要有音量大小，否则默认音量为0，检测的声音就是0了
        MainWindow::mutual->switchPage();//主线程开始录音
        MainWindow::mutual->m_pStackedWidget->setCurrentIndex(1);//mini模式切换至两个按钮页面
        recordStackedWidget->setCurrentIndex(1);//主界面切换至两个按钮页面
    }
    else
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                 ,tr("There is audio playing, please stop after recording!"),QMessageBox::Yes );
        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
        WrrMsg->exec();
        return ;
    }

}
