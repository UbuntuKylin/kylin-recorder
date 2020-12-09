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
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "QPixmap"
#include <QLabel>
#include <QSlider>
#include <qtextcodec.h>//保存中文路径时用上此头文件
#include <QMessageBox>

#include <QFileDialog>
#define BufferSize         35280
//const qint64 TIME_TRANSFORM = 1000 * 1000;              // 微妙转秒;
#define rectangleCount 65//矩形条个数
MainWindow *MainWindow::mutual = nullptr;//初始化！！！
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    checkSingle();//检查单例模式
    mutual = this;
    defaultPathData = new QGSettings(KYLINRECORDER);

    int WIDTH = 800;
    int HEIGHT = 460;
    this ->setFixedSize(WIDTH,HEIGHT);
    this ->setWindowIcon(QIcon(":/svg/svg/recording_128.svg"));
    //屏幕中间
    this->move((QApplication::desktop()->width() -WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);

    setButton = new QToolButton(this);
    miniButton = new QToolButton(this);
    minButton = new QToolButton(this);
    maxButton = new QToolButton(this);
    closeButton = new QToolButton(this);
    recordButton = new QToolButton(this);
    list = new QListWidget(this);

//  list->installEventFilter(this);//安装事件过滤器
//  list->setMouseTracking(true);
    list->setStyleSheet("QListWidget::item:selected{border:none;}"
                        "QListWidget::item:hover{background: rgb(255, 255, 255);}"
                        "QScrollBar{background-color:white;border-radius:2px;width:4px;}"
                        "QScrollBar::handle:vertical{background-color:#888888;border-radius:2px;width:4px;min-height:20px;}"
                        );

    connect(list,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(slotItemEntered(QListWidgetItem*)));
    connect(list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(slotOnItemDoubleClicked(QListWidgetItem*)));
    itemswindow = new ItemsWindow(this);


    pTimer = new QTimer;//第二个页面的控件初始化
    showTimelb = new QLabel(this);
    voicelb = new QLabel(this);
    slider = new QSlider(this);
    stopButton = new QToolButton(this);
    play_pauseButton = new QToolButton(this);

    ui_2Wid = new QWidget();//第二个页面布局
    timeWid = new QWidget();
    waveWid = new QWidget();//波形图
    voiceWid = new QWidget();
    playWid = new QWidget();

    layout1 = new QHBoxLayout();
    layout3 = new QHBoxLayout();
    layout4 = new QHBoxLayout();
    ui_2Layout = new QVBoxLayout();
    //线程的使用
    myThread=new MyThread;//子线程
    thread=new QThread;//主线程
    //先初始化数组都为0，2020.11.12先隐藏此功能
    for(int i=0;i<110;i++)
    {
        valueArray[i]=0;
    }
    connect(this,&MainWindow::startRecord,myThread,&MyThread::record_pressed);
    connect(myThread,&MyThread::recordPaint,this,&MainWindow::recordPaint);//绘波形图
    connect(myThread,&MyThread::changeVoicePicture,this,&MainWindow::changeVoicePicture);
    connect(this,&MainWindow::stopRecord,myThread,&MyThread::stop_btnPressed);
    connect(this,&MainWindow::playRecord,myThread,&MyThread::playRecord);
    connect(this,&MainWindow::pauseRecord,myThread,&MyThread::pauseRecord);

    connect(pTimer,&QTimer::timeout,this,&MainWindow::updateDisplay);
    connect(slider, SIGNAL(valueChanged(int)),myThread, SLOT(OnSliderValueChanged(int)));
    connect(stopButton,&QToolButton::clicked,this,&MainWindow::stop_clicked);
    connect(play_pauseButton,&QToolButton::clicked,this,&MainWindow::play_pause_clicked);

    qDebug()<<"主线程:"<<QThread::currentThread();

    myThread->moveToThread(thread);
    thread->start();
    //qDebug()<<"子线程:";
    //点击关闭时把线程关掉
    connect(closeButton,&QToolButton::clicked,[=](){
        thread->quit();
          thread->wait();
    });
    //setWindowFlags((Qt::FramelessWindowHint));//设置窗体无边框

    setWindowTitle(tr("kylin-recorder"));
    lb=new QLabel(this);
    piclb=new QLabel(this);//窗体左上角图片Label

    piclb->setStyleSheet("QLabel{border-image: url(:/png/png/recording_32.png);}");
    piclb->setFixedSize(25,25);
    lb->setText(tr("kylin-recorder"));//？字体待修改
    lb->setStyleSheet("font-size:14px;");//修改字体显示
    lb->setFixedHeight(25);
    fileListlb=new QLabel(this);
    fileListlb->setText(tr("  File List  "));
    fileListlb->setStyleSheet("font: bold; font-size:18px;");//修改字体显示

    seatlb=new QLabel(this);
    seatlb->setText("  ");//占位

    recordButton->setIcon(QIcon(":/png/png/recording_128.png"));
    recordButton->setIconSize(QSize(128,128));//重置图标大小
    connect(miniButton, &QToolButton::clicked,this,&MainWindow::miniShow);
    connect(minButton, &QToolButton::clicked,this,&MainWindow::minShow);
    connect(maxButton, &QToolButton::clicked,this,&MainWindow::maxShow);
    connect(closeButton,&QToolButton::clicked,this,&MainWindow::close);
    connect(recordButton, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(setButton, &QToolButton::clicked, this, &MainWindow::goset);


    mainWid = new QWidget();//主wid
    titleLeftWid = new QWidget();
    titleRightWid = new QWidget();
    recordButtonWid = new QWidget();//录制界面的按钮Wid
    listWid = new QWidget();//列表Wid
    leftMainWid = new QWidget();//主左Wid
    rightMainWid = new QWidget();//主右Wid
    listLayout = new QVBoxLayout();

    listLayout->addWidget(fileListlb);
    listLayout->addWidget(list);
    listLayout->setMargin(0);
//    listLayout->setSpacing(4);

    listWid->setLayout(listLayout);

    titleLeftLayout = new QHBoxLayout();//左标题布局
    titleRightLayout = new QHBoxLayout();//右标题布局
    m_pStackedLayout = new QStackedLayout();//堆栈布局
    leftMainWidLayout = new QVBoxLayout();//主左布局
    rightMainWidLayout = new QVBoxLayout();//主右布局
    mainLayout = new QHBoxLayout();//主窗体的布局

    MainWindowLayout();//主窗体布局方法
    initThemeGsetting();
    initFunctionGsetting();
    list->setViewMode(QListView::ListMode);
    list->setAttribute(Qt::WA_TranslucentBackground);
}

MainWindow::~MainWindow()
{

}

void MainWindow::MainWindowLayout()
{
    titleLeftLayout->addWidget(piclb);
    titleLeftLayout->addWidget(lb);

    titleLeftWid->setLayout(titleLeftLayout);
    titleLeftWid->setFixedHeight(35);
//  titleLeftWid->setStyleSheet("background-color:pink;");//测试用法

    titleRightLayout->addWidget(seatlb);//占位用的lb
    titleRightLayout->addWidget(setButton);
    titleRightLayout->addWidget(miniButton);
    titleRightLayout->addWidget(minButton);
    //titleRightLayout->addWidget(maxButton);//2020.11.20禁止最大化功能
    titleRightLayout->addWidget(closeButton);
    titleRightLayout->setSpacing(1);
    titleRightLayout->setMargin(5);
    titleRightWid->setLayout(titleRightLayout);
    titleRightWid->setFixedHeight(35);

    m_pStackedLayout->addWidget(recordButton);
    recordButtonWid->setLayout(m_pStackedLayout);

    leftMainWidLayout->addWidget(titleLeftWid);
    leftMainWidLayout->addWidget(recordButtonWid,0,Qt::AlignCenter);
    leftMainWidLayout->setMargin(0);
    leftMainWid->setLayout(leftMainWidLayout);
    leftMainWid->setStyleSheet("background-color:#E5E5E5;opacity:0.1;"
                               "border-top-right-radius:0px;"
                               "border-bottom-right-radius:0px;");
    leftMainWid->setFixedWidth(360);


    rightMainWidLayout->addWidget(titleRightWid);
    rightMainWidLayout->addWidget(listWid);
    rightMainWidLayout->setSpacing(0);
    rightMainWidLayout->setMargin(3);
    rightMainWid->setLayout(rightMainWidLayout);
    //rightMainWid->setFixedSize(430,450);
    //rightMainWid->setStyleSheet("background-color:purple;");


    mainLayout->addWidget(leftMainWid);
    mainLayout->addWidget(rightMainWid);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    mainWid->setLayout(mainLayout);

    this->setCentralWidget(mainWid);
    this->setAttribute(Qt::WA_TranslucentBackground);//主窗体透明
    this->setStyleSheet("border-radius:12px;" );//主窗体圆角(注意：窗体透明与主窗体圆角要搭配使用否则无效)
}

void MainWindow::minShow()
{
    this->showMinimized();
}
void MainWindow::maxShow()
{
    if(isMax)
    {
        this->showNormal();
        isMax = false;
    }
    else
    {
        this->showFullScreen();
        isMax = true;
    }

}
void MainWindow::themeWindow(QString themeColor)
{
    if(themeColor == "ukui-dark"||themeColor=="ukui-black")
    {
        listWid->setStyleSheet("background-color:#222222;");//后期适配主题颜色
        //list->setAttribute(Qt::WA_TranslucentBackground);
        leftMainWid->setStyleSheet("background-color:#141414;"
                                   "border-top-right-radius:0px;"
                                   "border-bottom-right-radius:0px;");
        //主界面
        mainWid->setObjectName("mainWid");//设置命名空间
        mainWid->setStyleSheet("#mainWid{background-color:#141414;}");//自定义窗体(圆角+背景色)
        //mini主界面
        mini.miniWid->setStyleSheet("border-radius:6px;background-color:#222222;");//后期适配主题颜s;
        mini.line->setStyleSheet("border-radius: 6px; background-color:black;");
        mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
                                           "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
                                           "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");

        //设置界面
        set.mainWid->setObjectName("setMainWid");//设置命名空间
        set.mainWid->setStyleSheet("#setMainWid{border-radius:12px;background-color:#131314;border:1px solid #262626;}");//自定义窗体(圆角+背景色)
        //设置界面


        rightMainWid->setStyleSheet("background-color:#222222;"
                                    "border-top-left-radius:0px;"
                                    "border-bottom-left-radius:0px;");
        itemswindow->setStyleSheet("background-color:#222222;"
                                "border-top-right-radius:0px;"
                                "border-bottom-right-radius:0px;");//后期适配主题颜s;
    }
    else
    {
        listWid->setStyleSheet("background-color:#FFFFFF;");//后期适配主题颜色
        mini.miniWid->setStyleSheet("border-radius:6px;background-color:#FBEFED;");//后期适配主题颜s;
        mini.line->setStyleSheet("border-radius: 6px; background-color:#EFE4E1;");
        //初始按钮为暂停按钮。因为只要点击录音按钮就要开始录制了
        mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                                 "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                                 "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");


        leftMainWid->setStyleSheet("background-color:#E5E5E5;opacity:0.1;"
                                   "border-top-right-radius:0px;"
                                   "border-bottom-right-radius:0px;");

        rightMainWid->setStyleSheet("background-color:#FFFFFF;"
                                    "border-top-left-radius:0px;"
                                    "border-bottom-left-radius:0px;");

        //设置界面
        set.mainWid->setObjectName("setMainWid");//设置命名空间
        set.mainWid->setStyleSheet("#setMainWid{border-radius:12px;background-color:#FFFFFF;border:1px solid #262626;}");//自定义窗体(圆角+背景色)
        //设置界面
        //主界面
        mainWid->setObjectName("mainWid");//设置命名空间
        mainWid->setStyleSheet("#mainWid{background-color:#FFFFFF;border:1px solid #262626;}");//自定义窗体(圆角+背景色)
         //主界面
        itemswindow->setStyleSheet("border-top-right-radius:0px;"
                            "border-bottom-right-radius:0px;");//自定义窗体(圆角+背景色)

    }
}
void MainWindow::themeButton(QString themeColor)
{
    if(themeColor == "ukui-dark"||themeColor=="ukui-black")
    {
        //list->setAttribute(Qt::WA_TranslucentBackground);
        list->setStyleSheet("QListWidget::item:selected{border:none;}"
                            "QListWidget::item:hover{background-color:#222222;}"
                            "QScrollBar{background-color:white;border-radius:2px;width:4px;}"
                            "QScrollBar::handle:vertical{background-color:#888888;border-radius:2px;width:4px;min-height:20px;}"
                            );
        //设置按钮
        setButton->setIcon(QIcon(":/svg/svg/open-menu.svg"));
        setButton->setFixedSize(30,30);
        setButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#D9D9D9;opacity:0.15;}"
                                 "QToolButton:pressed{background-color:#ffffff;opacity:0.20;}");
        //mini按钮
        miniButton->setFixedSize(30,30);
        miniButton->setStyleSheet("QToolButton{image:url(:/png/png/mini.png);border-radius:4px;}"
                                     "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                     "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //最小化按钮
        minButton->setIcon(QIcon(":/svg/svg/dark-window-min.svg"));
        minButton->setFixedSize(30,30);
        minButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //最大化按钮
        maxButton->setIcon(QIcon(":/svg/svg/dark-window-maximize.svg"));
        maxButton->setFixedSize(30,30);
        maxButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //关闭按钮
        closeButton->setIcon(QIcon(":/svg/svg/dark-window-close.svg"));
        closeButton->setFixedSize(30,30);
        closeButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                   "QToolButton:hover{background-color:#F04234}"
                                   "QToolButton:pressed{background-color:#D83436}");
        recordButton->setStyleSheet("background-color:#141414;"
                                    "border-radius:64px;");
        stopButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_stop.png);}"
                                  "QToolButton:hover{image: url(:/png/png/stop_hover.png);}"
                                  "QToolButton:pressed{image: url(:/png/png/stop_click.png);}");

        if(strat_pause)
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_start.png);}"
                        "QToolButton:hover{image: url(:/png/png/dark_start_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/dark_start_click.png);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini_white.png);}"
                           "QToolButton:hover{image:url(:/png/png/start_mini_hoverWhite.png);}"
                           "QToolButton:pressed{image:url(:/png/png/start_mini_click.png);}");
        }
        else
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_pause.png);}"
                        "QToolButton:hover{image: url(:/png/png/dark_pause_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/dark_pause_click.png);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
                                               "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");


        }
    }
    else
    {
        //list->setAttribute(Qt::WA_TranslucentBackground);
        list->setStyleSheet("QListWidget::item:selected{border:none;}"
                            "QListWidget::item:hover{background: rgb(255, 255, 255);}"
                            "QScrollBar{background-color:white;border-radius:2px;width:4px;}"
                            "QScrollBar::handle:vertical{background-color:#888888;border-radius:2px;width:4px;min-height:20px;}"
                            );
        //设置按钮
        setButton->setIcon(QIcon(":/svg/svg/open-menu.svg"));
        setButton->setFixedSize(30,30);
        setButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //mini按钮
        miniButton->setFixedSize(30,30);
        miniButton->setStyleSheet("QToolButton{image:url(:/png/png/mini.png);border-radius:4px;}"
                                     "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                     "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //最小化按钮
        minButton->setIcon(QIcon(":/svg/svg/window-min.svg"));
        minButton->setFixedSize(30,30);
        minButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //最大化按钮
        maxButton->setIcon(QIcon(":/svg/svg/window-maximize.svg"));
        maxButton->setFixedSize(30,30);
        maxButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
        //关闭按钮
        closeButton->setIcon(QIcon(":/svg/svg/window-close.svg"));
        closeButton->setFixedSize(30,30);
        closeButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                   "QToolButton:hover{background-color:#F04234}"
                                   "QToolButton:pressed{background-color:#D83436}");
        recordButton->setStyleSheet("background-color:#E5E5E5;"
                                    "border-radius:64px;");
        stopButton->setStyleSheet("QToolButton{image: url(:/png/png/stop.png);}"
                                  "QToolButton:hover{image: url(:/png/png/stop_hover.png);}"
                                  "QToolButton:pressed{image: url(:/png/png/stop_click.png);}");

        if(strat_pause)
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/start.png);}"
                                         "QToolButton:hover{image: url(:/png/png/start_hover.png);}"
                                         "QToolButton:pressed{image: url(:/png/png/start_click.png);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini.png);}"
                           "QToolButton:hover{image:url(:/png/png/start_mini_hover.png);}"
                           "QToolButton:pressed{image:url(:/png/png/start_mini_click.png);}");
        }
        else
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/pause.png);}"
                        "QToolButton:hover{image: url(:/png/png/pause_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/pause_click.png);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                                               "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");

        }

    }

}

QString MainWindow::playerTotalTime(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileinfo(filePath);
    qint64 fileSize ;
    qint64 time;
    QString timeStr;
    if (file.open(QIODevice::ReadOnly))
    {

        if(fileinfo.suffix().contains("mp3"))
        {
            fileSize = file.size();
            qDebug()<<file.size();
            time = fileSize/16000;//时间长度=文件大小/比特率
            QTime totalTime(time/3600,(time%3600)/60,time%60);
            timeStr=totalTime.toString("hh:mm:ss");

        }
        else if(fileinfo.suffix().contains("m4a"))
        {
            fileSize = file.size();
            time = fileSize/4550;//时间长度=文件大小/比特率
            QTime totalTime(time/3600,(time%3600)/60,time%60);
            timeStr=totalTime.toString("hh:mm:ss");
        }
        else if(fileinfo.suffix().contains("wav"))
        {
            fileSize = file.size();
            time = fileSize/96000;//时间长度=文件大小/比特率
            QTime totalTime(time/3600,(time%3600)/60,time%60);
            timeStr=totalTime.toString("hh:mm:ss");
        }
        file.close();
        return timeStr;
    }


}

//初步配置文件
void MainWindow::initThemeGsetting()
{
    themeData = new QGSettings(FITTHEMEWINDOW);
    connect(themeData, &QGSettings::changed, this, [=]  ()
    {
        theme = themeData->get("style-name").toString();
        themeStyle(theme);
    });
    themeStyle(themeData->get("style-name").toString());

}

void MainWindow::initFunctionGsetting()
{
    qDebug() <<"初始化";
    int  m=myThread->readNumList()-1;
    //qDebug()<<m;
    QStringList listRecordPath=myThread->readPathCollected().split(",");
    for(int i=1;i<=m;i++)
    {

        QString str=listRecordPath.at(i);
        //qDebug()<<str;
        QFileInfo fileinfo(str);
        QString filesuffix = fileinfo.suffix();//判断文件后缀
        //qDebug()<<fileinfo.isFile();//判断是否为文件，是文件就存在了,因为在本地删除后，同步文件列表下才打开时那个文件也没了
        //qDebug()<<filesuffix;
        if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
        {
            qDebug()<<"文件存在!";
            //qDebug()<<str.split("/").last();
            //qDebug()<<QDesktopServices::openUrl(QUrl::fromLocalFile(str));//打开指定路径文件夹
            slotListItemAdd(str,i);//每当配置文件中有路径时就在list中更新一下
        }
        else
        {
            qDebug()<<str<<"文件或被删除！";
            QString subStr=","+str;//子串
            /*
             * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串内容
             */
            QString oldStr=defaultPathData->get("recorderpath").toString();
            int pos=oldStr.indexOf(subStr);
            //qDebug()<<pos<<" "<<oldStr;
            //qDebug()<<oldStr.mid(pos,str.length()+1);
            QString newStr = oldStr.remove(pos,str.length()+1);
            myThread->writePathCollected(newStr);
            myThread->writeNumList(myThread->readNumList()-1);
            qDebug()<<myThread->readPathCollected();
        }

    }
}

void MainWindow::themeStyle(QString themeColor)
{
    qDebug()<<themeColor;
    themeButton(themeColor);//控件类切换
    themeWindow(themeColor);
}

void MainWindow::checkSingle()//检查单例模式
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    //兼容VNC的单例模式
    int fd = open(QString(homePath.at(0) + "/.config/kylin-recorder%1.lock").arg(getenv("DISPLAY")).toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) { exit(1); }
    if (lockf(fd, F_TLOCK, 0)) {
        qDebug()<<"Can't lock single file, kylin-recorder is already running!";
        exit(0);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        this->isPress = true;
        this->winPos = this->pos();
        this->dragPos = event->globalPos();
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    this->isPress = false;
    this->setCursor(Qt::ArrowCursor);

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(this->isPress){
        this->move(this->winPos - (this->dragPos - event->globalPos()));
        this->setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void MainWindow::miniShow()
{
    this->hide();
    mini.show();
}

//开始和暂停
void MainWindow::play_pause_clicked()
{
    static QTime pauseTime;
    if(strat_pause)
    {
       emit playRecord();
       cut = QTime::currentTime();//记录开始时的时间
       int t = pauseTime.secsTo(cut);//点击暂停时时间与点击恢复计时的时间差值
       //qDebug()<<t;
       baseTime = baseTime.addSecs(t);
       pTimer->start(1);
       mini.baseTime= mini.baseTime.addSecs(t);
       mini.pTimer->start(1);
       mini.start_pause= false;
       strat_pause=false;
       if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
       {
           mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini_white.png);}"
                                              "QToolButton:hover{image:url(:/png/png/pause_mini_hoverWhite.png);}"
                                              "QToolButton:pressed{image:url(:/png/png/pause_mini_clickWhite2.png);}");
           play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_pause.png);}"
                       "QToolButton:hover{image: url(:/png/png/dark_pause_hover.png);}"
                       "QToolButton:pressed{image: url(:/png/png/dark_pause_click.png);}");
       }
       else
       {
           //点击完之后要修改悬停样式。
           mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                                                            "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                                                            "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");
           play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/pause.png);}"
                       "QToolButton:hover{image: url(:/png/png/pause_hover.png);}"
                       "QToolButton:pressed{image: url(:/png/png/pause_click.png);}");
       }

    }
    else
    {
        qDebug()<<"pause";
        emit pauseRecord();
        pTimer->stop();
        mini.pTimer->stop();
        //mini.pTimer->stop();
        pauseTime = QTime::currentTime();//记录一下当前暂停时的时间
        qDebug()<<pauseTime;
        mini.start_pause = true;
        strat_pause = true;
        //点击完之后要修改悬停样式。
        if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini_white.png);}"
                                               "QToolButton:hover{image:url(:/png/png/start_mini_hoverWhite.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/start_mini_clickWhite2.png);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_start.png);}"
                        "QToolButton:hover{image: url(:/png/png/dark_start_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/dark_start_click.png);}");
        }
        else
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini.png);}"
                                               "QToolButton:hover{image:url(:/png/png/start_mini_hover.png);}"
                                               "QToolButton:pressed{image:url(:/png/png/start_mini_click.png);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/start.png);}"
                        "QToolButton:hover{image: url(:/png/png/start_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/start_click.png);}");
        }


    }

}

void MainWindow::recordPaint(int value)
{
    maxNum.prepend(value/300);//将元素插入到Vector的开始
    maxNum = maxNum.mid(0,rectangleCount);
    for(int i = 0;i<rectangleCount;i++)
    mywave.at(i)->setValue(maxNum.at(i));
}

void MainWindow::stop_clicked()//停止按钮
{
    if(stop)
    {
        pTimer->stop();//计时停止
        mini.pTimer->stop();
        emit stopRecord();
        //停止之后把数组清0
        for(int i = 0;i<110;i++)
        {
            valueArray[i] = 0;
        }
        if(strat_pause)
        {
            strat_pause = false;
        }
        //停止按钮触发后的界面
        play_pauseButton->setEnabled(false);
        if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_start.png);}"
                        "QToolButton:hover{image: url(:/png/png/dark_start_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/dark_start_click.png);}");
        }
        else
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/start.png);}"
                        "QToolButton:hover{image: url(:/png/png/start_hover.png);}"
                        "QToolButton:pressed{image: url(:/png/png/start_click.png);}");
        }

        showTimelb->setText("00:00:00");//主界面计时停止
        mini.timelb->setText("00:00:00");//mini模式计时停止
        //showTimelb->setStyleSheet("font: bold; font-size:20px;");
        stop=false;
        m_pStackedLayout->setCurrentIndex(0);
        mini.recordStackedLayout->setCurrentIndex(0);       
    }

}
void MainWindow::updateDisplay()
{

//    1.点击开始后获取到当前的时间并且赋值给baseTime
//    2.启动定时器,间隔1s
//    3.槽函数中再次获取当前的时间currTime
//    4.计算两个时间的差值t
//    5.声明一个showTime对象给他加上t的差值
//    6.格式化后设置显示
    QTime currTime = QTime::currentTime();
    int t = this->baseTime.secsTo(currTime);
    QTime showTime(0,0,0);
    showTime = showTime.addSecs(t);
    this->timeStr = showTime.toString("hh:mm:ss");
    this->showTimelb->setText(timeStr);
}
void MainWindow::mainWindow_page2()
{
    maxNum.clear();
    for(int i=0;i<rectangleCount;i++)maxNum.append(0);
    emit startRecord();
    slider->setValue(50);//初始音量为50
    voicelb->setStyleSheet("QLabel{image: url(:/png/png/voice.png);}");//初始图标为有声图标
    play_pauseButton->setIconSize(QSize(56,56));//重置图标大小
    play_pauseButton->setEnabled(true);//按下后，开始录音可以按暂停
    if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
    {
        play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_pause.png);}"
                                        "QToolButton:hover{image: url(:/png/png/dark_pause_hover.png);}"
                                        "QToolButton:pressed{image: url(:/png/png/dark_pause_click.png);}");
        stopButton->setStyleSheet("QToolButton{image: url(:/png/png/dark_stop.png);}"
                                  "QToolButton:hover{image: url(:/png/png/dark_hover_stop.png);}"
                                  "QToolButton:pressed{image: url(:/png/png/dark_click_stop.png);}");
    }
    else
    {
        play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/pause.png);}"
                                        "QToolButton:hover{image: url(:/png/png/pause_hover.png);}"
                                        "QToolButton:pressed{image: url(:/png/png/pause_click.png);}");
        stopButton->setStyleSheet("QToolButton{image: url(:/png/png/stop.png);}"
                                  "QToolButton:hover{image: url(:/png/png/stop_hover.png);}"
                                  "QToolButton:pressed{image: url(:/png/png/stop_click.png);}");
    }
    baseTime = baseTime.currentTime();
    pTimer->start(1);
    mini.baseTime = mini.baseTime.currentTime();
    mini.pTimer->start(1);
    stop=true;

    stopButton->setIconSize(QSize(56,56));//重置图标大小
    //pauseButton->setStyleSheet("border: 0px");//消除边框，取消点击效果
    showTimelb->setText("00:00:00");
    showTimelb->setStyleSheet("font: bold; font-size:18px;");
    voicelb->setFixedSize(21,21);

    slider->setOrientation(Qt::Horizontal);
    slider->setStyleSheet("QSlider::handle:horizontal{width:12px;background-color:#404040;margin:-5px 0px -5px 0px;border-radius:6px;}"
                          "QSlider::groove:horizontal{height:3px;background-color:rgb(219,219,219);}"
                          "QSlider::add-page:horizontal{background-color:rgb(219,219,219);}"
                          "QSlider::sub-page:horizontal{background-color:#404040;}");

    layout1->addWidget(showTimelb,0,Qt::AlignCenter);//在布局的中央
    timeWid->setLayout(layout1);
    layout2=new QHBoxLayout();//波形图所在的布局
    for (int i=0;i<rectangleCount;i++)//频率直方图
    {
        myWave *wave=new myWave(this);//每次都要初始化一个矩形框
//      wave->setMaximumWidth(100);
        wave->setRange(0,100);
        mywave.push_back(wave);
        layout2->addWidget(wave);

    }
    layout2->setSpacing(2);
    waveWid->setLayout(layout2);
    waveWid->setFixedHeight(120);

    layout3->addWidget(voicelb);
    layout3->addWidget(slider);
    voiceWid->setLayout(layout3);

    layout4->addWidget(stopButton);
    layout4->addWidget(play_pauseButton);
    playWid->setLayout(layout4);


    ui_2Layout->addWidget(timeWid);
    ui_2Layout->addWidget(waveWid);
    ui_2Layout->addWidget(voiceWid);
    ui_2Layout->addWidget(playWid);

    ui_2Wid->setLayout(ui_2Layout);
    m_pStackedLayout->addWidget(ui_2Wid);


}
//与堆叠布局搭配使用效果更佳
void MainWindow::switchPage()
{
//    for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
//    {
//        if(deviceInfo.deviceName().contains("auto_null"))
//        {
//            WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning"),tr("No input device detected！"),QMessageBox::Yes );
//            WrrMsg->button(QMessageBox::Yes)->setText("OK");
//            WrrMsg->exec();
//            return;
//        }
//        if(deviceInfo.deviceName().contains("alsa_input"))
//        {
//            qDebug()<<"录音设备是:"<<deviceInfo.deviceName();
//            break;
//        }

//    }
    mainWindow_page2();

    int nCount = m_pStackedLayout->count();
    int nIndex = m_pStackedLayout->currentIndex();
    // 获取下一个需要显示的页面索引
    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;
    mini.recordStackedLayout->setCurrentIndex(nIndex);//切换至录音按钮
    m_pStackedLayout->setCurrentIndex(nIndex);
}
void MainWindow::changeVoicePicture()
{
    if(slider->value()==0)
        voicelb->setStyleSheet("QLabel{image: url(:/png/png/Silence.png);}");
    else
        voicelb->setStyleSheet("QLabel{image: url(:/png/png/voice.png);}");
}
void MainWindow::goset()
{
    set.show();
}
void MainWindow::handlingSlot(QString handling)
{
    ItemsWindow *itemswindow = new ItemsWindow(this);
    //添加当前录音文件的时长
    itemswindow->timelengthlb->setText(handling);
    //qDebug()<<itemswindow->timelengthlb->text();
}
void MainWindow::slotListItemAdd(QString fileName,int i)
{
    qDebug()<<"更新";
    ItemsWindow *itemswindow = new ItemsWindow(this);//初始化Item录音文件类必须加this,
                                                     //因为后期要判断子类的子控件
    itemswindow->recordFileNamelb->setText(tr("recorder")+QString::number(i));
    //添加当前录音文件的文件名(以时间命名)
    itemswindow->dateTimelb->setText(fileName.split("/").last());
    //添加当前录音文件的时长
    itemswindow->timelengthlb->setText(playerTotalTime(fileName));
    QListWidgetItem *aItem = new QListWidgetItem(list);//添加自定义的item
    list->setItemWidget(aItem,itemswindow->clipperstackWid);
    //list->addItem(aItem);
    aItem->setSizeHint(QSize(0,60));
}
void MainWindow::slotOnItemDoubleClicked(QListWidgetItem *item)
{

}
void MainWindow::slotItemEntered(QListWidgetItem *item)
{
//    qDebug()<<list->currentRow();
//    QWidget* getWid =list->itemWidget(item);
//    if(item->isSelected()&&itemSelect==0)
//    {
//        QList<QLabel*> labelList = getWid->findChildren<QLabel*>();  // 获取所有的QToolButton
//        foreach(QLabel *label, labelList)
//        {
////            label->setStyleSheet("color:red;"
////                                 "font-size:14px;");
//        }
//        itemSelect=1;

//    }
//    else if(item->isSelected()&&itemSelect==1)
//    {
//        QList<QLabel*> labelList = getWid->findChildren<QLabel*>();  // 获取所有的QToolButton
//        foreach(QLabel *label, labelList)
//        {
////            label->setStyleSheet("color:black;"
////                                 "font-size:14px;");
//        }
//        itemSelect=0;
//    }



}


