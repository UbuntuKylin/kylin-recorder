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

#include "xatom-helper.h"
#define BufferSize         35280
//const qint64 TIME_TRANSFORM = 1000 * 1000;              // 微妙转秒;
#define rectangleCount 79//矩形条个数

MainWindow *MainWindow::mutual = nullptr;//初始化！！！
MainWindow::MainWindow(QStringList str,QWidget *parent)
    :QMainWindow(parent)
{
    mutual = this;
    defaultPathData = new QGSettings(KYLINRECORDER);
    // 用户手册功能
    mDaemonIpcDbus = new DaemonDbus();

    checkSingle(str);//检查单例模式
    qDebug()<<"sssssssssssss"<<str;

    if(!argName.isEmpty())
    {
        qDebug()<<"argName1111111"<<argName;
        int num = argName.size();
        switch (num) {
        case 1:
            command_Control(argName[0]);
            break;
        default:
            break;
        }
    }
    isFirstObject = false;//可以接收外部命令
    initDbus();//初始化插拔信号和s3/s4的dbus
    initMainWindow();//初始化主界面
    setTwoPageWindow();//设置点击按钮切换的第二个页面

//    this->setWindowFlags((Qt::FramelessWindowHint));//设置窗体无边框**加窗管协议后要将此注释调**
    MainWindowLayout(); //主窗体布局方法
    initThemeGsetting();//初始化主题配置文件
    mainWindow_page2(); //必须加上初始化第二个主页面(此函数有两处需要被调用:构造函数+切换页面时)
    updateGsetting_ListWidget();

    qDebug()<<"主线程:"<<QThread::currentThread();
    myThread->moveToThread(thread);
    thread->start();
    //qDebug()<<"子线程:";
    //点击关闭时把线程关掉
    connect(closeButton,&QToolButton::clicked,[=](){
        thread->quit();
          thread->wait();
    });
    //这个分支要放在下面,对于执行命令时有些还没来得及实例化所以要放在后面

    mainWid->show();
}

void MainWindow::initDbus()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if(sessionBus.registerService("org.ukui.kylin_recorder"))
    {
        sessionBus.registerObject("/org/ukui/kylin_recorder",this,
                                  QDBusConnection::ExportAllContents);
        qDebug()<<"初始化DBUS成功";
    }
    else
        qDebug()<<"初始化DBUS失败";
    //S3 S4策略
    QDBusConnection::systemBus().connect(QString("org.freedesktop.login1"),
                                         QString("/org/freedesktop/login1"),
                                         QString("org.freedesktop.login1.Manager"),
                                         QString("PrepareForShutdown"), this,
                                         SLOT(onPrepareForShutdown(bool)));
    QDBusConnection::systemBus().connect(QString("org.freedesktop.login1"),
                                         QString("/org/freedesktop/login1"),
                                         QString("org.freedesktop.login1.Manager"),
                                         QString("PrepareForSleep"), this,
                                         SLOT(onPrepareForSleep(bool)));
    //插拔耳机的信号
    QDBusConnection::sessionBus().connect(QString(), QString( "/"), "org.ukui.media", "DbusSingleTest",this, SLOT(inputDevice_get(QString)));
}

//初始化主界面
void MainWindow::initMainWindow()
{
    mainWid = new QWidget();//主wid
    int WIDTH = 800 ;
    int HEIGHT = 460 ;
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(mainWid->winId(), hints);
    mainWid->installEventFilter(this);

    mainWid ->setFixedSize(WIDTH,HEIGHT);
    mainWid ->setWindowIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/svg/svg/recording_128.svg")));
    mainWid ->setWindowTitle(tr("Recorder"));
    //屏幕中间老方法
    //    this->move((QApplication::desktop()->width() -WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);
    //显示在活动屏幕中间新方法
    QScreen *screen = QGuiApplication::primaryScreen();
    mainWid ->move((screen->geometry().width() - WIDTH) / 2,(screen->geometry().height() - HEIGHT) / 2);

    titleLeftWid = new QWidget();
    titleRightWid = new QWidget();
    mainLayout = new QHBoxLayout();//主窗体的布局
    m_pStackedWidget = new QStackedWidget();//录音图标和波形图的堆栈Wid
    listWid = new QWidget();//列表Wid
    leftMainWid = new QWidget();//主左Wid
    rightMainWid = new QWidget();//主右Wid
    listLayout = new QVBoxLayout();
    recordButtonWid = new QWidget();
    titleLeftLayout = new QHBoxLayout();//左标题布局
    titleRightLayout = new QHBoxLayout();//右标题布局

    leftMainWidLayout = new QVBoxLayout();//主左布局
    rightMainWidLayout = new QVBoxLayout();//主右布局

    appPicture=new QToolButton(this);//窗体左上角标题图片
    appPicture->setIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/png/png/recording_32.png")));
    appPicture->setFixedSize(24,24);
    appPicture->setIconSize(QSize(24,24));//重置图标大小
    lb=new QLabel(this);
    lb->setText(tr("Recorder"));//？字体待修改
    lb->setStyleSheet("font-size:14px;");//修改字体显示
    lb->setFixedHeight(24);

    //setButton已弃用
    setButton = new QPushButton(this);
    setButton->setIcon(QIcon::fromTheme("open-menu-symbolic"));//主题库的菜单图标
    menu = new QMenu(this);
    actionSet = new QAction(tr("Set"),menu);
    actionHelp = new QAction(tr("Help"),menu);
    actionAbout = new QAction(tr("About"),menu);

    //(三)按钮及下拉菜单
    menumodule = new menuModule(mainWid);
    menumodule->menuButton->setToolTip(tr("Menu"));
    menumodule->menuButton->setFixedSize(30,30);
    menumodule->menuButton->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    menumodule->menuButton->setAutoRaise(true);
    menumodule->menuButton->setProperty("isWindowButton", 0x1);
    menumodule->menuButton->setProperty("useIconHighlightEffect",0x2);
    menumodule->menuButton->setPopupMode(QToolButton::InstantPopup);//QToolButton必须加上的

    //mini按钮
    miniButton = new QToolButton(this);
    miniButton->setFixedSize(30,30);
    miniButton->setToolTip(tr("Mini"));
    miniButton->setProperty("isWindowButton", 0x1);
    miniButton->setProperty("useIconHighlightEffect", 0x2);
    miniButton->setAutoRaise(true);
    miniButton->setIcon(QIcon::fromTheme("ukui-mini"));//主题库的mini图标

    //最小化按钮
    minButton = new QToolButton(this);
    minButton->setFixedSize(30,30);
    minButton->setToolTip(tr("Minimize"));
    minButton->setProperty("isWindowButton", 0x1);
    minButton->setProperty("useIconHighlightEffect", 0x2);
    minButton->setAutoRaise(true);
    minButton->setIcon(QIcon::fromTheme("window-minimize-symbolic"));//主题库的最小化图标

    //最大化按钮
    maxButton = new QToolButton(this);
    maxButton->setToolTip(tr("Max"));
    maxButton->setProperty("isWindowButton", 0x1);
    maxButton->setProperty("useIconHighlightEffect", 0x2);
    maxButton->setAutoRaise(true);
    maxButton->setIcon(QIcon::fromTheme("window-maximize-symbolic"));//主题库的最小化图标

    //关闭按钮
    closeButton = new QToolButton(this);
    closeButton->setFixedSize(30,30);
    closeButton->setToolTip(tr("Close"));
    closeButton->setIcon(QIcon::fromTheme("window-close-symbolic"));//主题库的叉子图标
    closeButton->setProperty("isWindowButton", 0x2);
    closeButton->setProperty("useIconHighlightEffect", 0x8);
    closeButton->setAutoRaise(true);
    recordButton = new QPushButton(this);
    recordButton->setToolTip(tr("Recording"));
    recordButton->setIcon(QIcon(":/svg/svg/start-recording.svg"));
    recordButton->setFixedSize(128,128);
    recordButton->setIconSize(QSize(128,128));//重置图标大小

    connect(miniButton,   &QToolButton::clicked, this, &MainWindow::miniShow);//mini窗口
    connect(minButton,    &QToolButton::clicked, this, &MainWindow::minShow);//最小化窗口
    connect(maxButton,    &QToolButton::clicked, this, &MainWindow::maxShow);//最大化窗口
//    connect(closeButton,&QToolButton::clicked, mainWid, &MainWindow::close);//关闭
    connect(closeButton,  &QToolButton::clicked, this, &MainWindow::closeWindow);//关闭
    connect(actionSet,    &QAction::triggered,   this, &MainWindow::goset);//跳转设置界面
    //主界面按钮
    connect(recordButton, &QPushButton::clicked, this, &MainWindow::switchPage);//换页

}

//设置第二个页面
void MainWindow::setTwoPageWindow()
{
    //线程的使用
    myThread = new MyThread;//子线程
    thread = new QThread;//主线程
    //第二个页面的控件初始化
    pTimer = new QTimer;//定时器,用来记录
    limitTimer = new QTimer;//设置定时器为限制录音时长所用目前规定只允许录制不超过15分钟的录音.

    showTimelb = new QLabel(this);
    voiceBtn = new QToolButton(this);

    slider = new QSlider(this);
    stopButton = new QToolButton(this);
    stopButton->setToolTip(tr("stop"));
    play_pauseButton = new QToolButton(this);
    play_pauseButton->setToolTip(tr("pause/start"));

    ui_2Wid = new QWidget();//第二个页面wid
    showTimelbWid = new QWidget();//时间wid
    waveWid = new QWidget();//波形图wid
    voiceAndSliderWid = new QWidget();//音量wid
    controlPlay_PauseWid = new QWidget();

    showTimelbLayout = new QHBoxLayout();
    voiceAndSliderLayout = new QHBoxLayout();
    controlPlay_PauseLayout = new QHBoxLayout();
    ui_2Layout = new QVBoxLayout();
    playerCompoment = new QMediaPlayer;//播放组件
    playList = new QMediaPlaylist;//播放列表
    tipWindow = new TipWindow();

    setWindowTitle(tr("Recorder"));
    //主界面右侧listWidget
    fileListlb=new QLabel(this);
    fileListlb->setText(tr("File List"));
    fileListlb->setStyleSheet("font: bold; font-size:18px;");//修改字体显示
    fileListWid = new QWidget();
    fileListLayout = new QHBoxLayout();
    zeroFile_Messagelb = new QLabel(this);

    seatlb=new QLabel(this);
    seatlb->setText("  ");//占位

    list = new QListWidget(this);
    list->installEventFilter(this);//安装事件过滤器
    list->setSortingEnabled(true);
    list->sortItems(Qt::DescendingOrder);
    list->setViewMode(QListView::ListMode);

    //开始时也要把数组都初始化为0，2020.11.12先隐藏此功能
    for(int i=0;i<INIT_MAINWINDOW_RECTANGLE_COUNT;i++)
    {
        valueArray[i] = 0;
    }
    connect(this,    &MainWindow::startRecord, myThread, &MyThread::record_pressed);
//    connect(this,  &MainWindow::recordingSignal,MainWindow::,&ItemsWindow::getRecordingSlot);//发送录音时的信号
    connect(myThread,&MyThread::recordPaint,       this, &MainWindow::recordPaint);//绘波形图
    connect(myThread,&MyThread::changeVoicePicture,this, &MainWindow::changeVoicePicture);
    connect(this,    &MainWindow::stopRecord,  myThread, &MyThread::stop_btnPressed);
    connect(this,    &MainWindow::playRecord,  myThread, &MyThread::playRecord);
    connect(this,    &MainWindow::pauseRecord, myThread, &MyThread::pauseRecord);

    connect(pTimer,       &QTimer::timeout,         this,  &MainWindow::updateDisplay);
    connect(limitTimer,   &QTimer::timeout,         this,  &MainWindow::limitRecordingTime);
    connect(slider,    SIGNAL(valueChanged(int)),myThread, SLOT(OnSliderValueChanged(int)));
    connect(stopButton,&QToolButton::clicked,       this,  &MainWindow::stop_clicked);
    connect(play_pauseButton,&QToolButton::clicked, this,  &MainWindow::play_pause_clicked);

}

int MainWindow::command_Control(QString cmd1)
{
    qDebug()<<"命令:"<<cmd1;
    //990需求命令行控制
    if(isFirstObject&&!QFileInfo::exists(cmd1))//首个实例不接受文件以外的参数
    {
        if(cmd1 == "-c"||cmd1 =="-close")
        {
            qDebug()<<"-c"<<cmd1;
            this->close();
            exit(0);
        }
        qDebug()<<"首个实例不接受文件以外的参数"<<cmd1;
        isFirstObject = false;//可以接收其他命令
        return 0;
    }
    if(cmd1=="")//无参数，单例触发
    {
        //kwin接口唤醒
        KWindowSystem::forceActiveWindow(mainWid->winId());
        qDebug()<<"窗口置顶";
        return 0;
    }
    if(cmd1=="-s"||cmd1=="-start")//开始录音
    {
        //------录音------
        qDebug()<<"isRecording = "<<isRecording<<" "<<strat_pause;
        if(!isRecording&&!strat_pause)
        {
            qDebug()<<"可以录音";
            switchPage();
        }
        return 0;
    }
    if(cmd1=="-p"||cmd1=="-pause")//暂停
    {
        //------暂停------
        if(isRecording)
        {
            qDebug()<<isRecording<<" "<<strat_pause;
            qDebug()<<"录音暂停";
            play_pause_clicked();//检测到正在录音时才可以暂停录制
        }
        else
        {
            if(strat_pause)//因为暂停录音时strat_pause会置true
            {
                play_pause_clicked();//开始录制
            }
        }
        return 0;
    }
    if(cmd1=="-f"||cmd1=="-finish")//结束录音
    {
        qDebug()<<"isRecording = "<<isRecording<<"strat_pause = "<<strat_pause;
        if(isRecording||strat_pause)
        {
            qDebug()<<"可以结束";
            stop_clicked();
        }
        return 0;
    }
    if(cmd1=="-c"||cmd1=="-close")//结束录音
    {
        thread->quit();
        thread->wait();
        mainWid->close();
        mini.miniWid->close();
        return 0;
    }
}

//对于台式机，可以收到耳机插拔的DBus信号
void MainWindow::inputDevice_get(QString str)
{
    qDebug()<<"插拔";
    if(isRecording)
    {
        //录音时，插拔要停止录音,并生成文件
        stop_clicked();
    }
    else
    {
        qDebug()<<"应用没有在录音！";
        return;
    }
}

void MainWindow::onPrepareForShutdown(bool Shutdown)
{
    //目前只做事件监听，不处理
    qDebug()<<"onPrepareForShutdown"<<Shutdown;
}

//监听系统睡眠信号
void MainWindow::onPrepareForSleep(bool isSleep)
{
    //990
    //空指针检验
    //------此处空指针校验（如果用了指针）------
    //系统事件
    if(isSleep)//睡眠分支
    {
        if(isRecording)
        {
            play_pause_clicked();//检测到睡眠时要暂停录制
            qDebug()<<"睡眠！！！";
        }
    }
    else//唤醒分支
    {
        if(!isRecording)//如果没有录音则走此分支
        {
            if(strat_pause)//因为暂停录音时strat_pause会置true
            {
                play_pause_clicked();//检测到唤醒时要开始录制
                qDebug()<<"唤醒！！！";
            }
            //一种情况是压根就没开始录制他就睡眠了。因此就不会做其他事情
        }
    }
}

void MainWindow::handlingSlot(bool isOk)
{
//    WrrMsg = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("Transcoding..."),QMessageBox::Yes);
//    WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
//    WrrMsg->exec();
    tipWindow->move(mainWid->geometry().center() - tipWindow->rect().center());
    tipWindow->show();
    qDebug()<<"接收到界面消息!";

}

void MainWindow::closeWindow()
{

    if (isRecording == true)
    {
//        myThread->stop_saveDefault();
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                 ,tr("Please stop recording before closing!"),QMessageBox::Yes );
        WrrMsg->move(mainWid->geometry().center() - WrrMsg->rect().center());
        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
        WrrMsg->exec();
        return ;

    }else
    {
        thread->quit();
        thread->wait();
        mainWid->close();
        mini.miniWid->close();
    }

}

// 实现键盘响应
void MainWindow::keyPressEvent(QKeyEvent *event)
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

MainWindow::~MainWindow()
{
    delete itemswindow;
    QList<myWave*> list = this->findChildren<myWave*>();
    for(myWave* tmp:list)
    {
        tmp->deleteLater();
    }
    delete tipWindow;
}

void MainWindow::isFileNull(int n)
{
    qDebug()<<"**********"<<n;
    if(n == 0)
    {
        zeroFile_Messagelb->setParent(listWid);
        zeroFile_Messagelb->show();
    }
    else
    {
        zeroFile_Messagelb->hide();
    }
}

void MainWindow::MainWindowLayout()
{

    zeroFile_Messagelb->setFixedSize(200,50);
    zeroFile_Messagelb->setText(tr("None of the Recording File"));//列表无录音文件时
    zeroFile_Messagelb->move((listWid->width()-zeroFile_Messagelb->width()/2)/3,(listWid->height()-zeroFile_Messagelb->height()/3)/3);

    fileListLayout->addWidget(fileListlb);
    fileListLayout->setContentsMargins(31,0,0,10);//31,0,0,0改为31,0,0,10即不多不少显示6个
    fileListWid->setLayout(fileListLayout);

    listLayout->addWidget(fileListWid);
    listLayout->addWidget(list);
    listLayout->setMargin(2);
    listWid->setLayout(listLayout);

    titleLeftLayout->addWidget(appPicture);
    titleLeftLayout->addWidget(lb);
    titleLeftLayout->setSpacing(8);//内部按钮间距8px
    titleLeftLayout->setContentsMargins(8,8,0,0);
    titleLeftWid->setLayout(titleLeftLayout);
    titleLeftWid->setFixedHeight(33);
//  titleLeftWid->setStyleSheet("background-color:pink;");//测试用法

    titleRightLayout->addWidget(seatlb);//占位用的lb
//    titleRightLayout->addWidget(setButton);
    titleRightLayout->addWidget(menumodule->menuButton);
    titleRightLayout->addWidget(miniButton);
    titleRightLayout->addWidget(minButton);
//    titleRightLayout->addWidget(maxButton);//2020.11.20禁止最大化功能
    titleRightLayout->addWidget(closeButton);
    titleRightLayout->setSpacing(4);//内部按钮间距4px
    titleRightLayout->setContentsMargins(0,4,4,0);
    titleRightWid->setLayout(titleRightLayout);
//    titleRightWid->setFixedHeight(38);

    recordButtonLayout = new QVBoxLayout();
    recordButtonLayout->addWidget(recordButton,0,Qt::AlignCenter);//录音按钮所在Wid

    recordButtonWid->setLayout(recordButtonLayout);
    recordButtonWid->setFixedHeight(210);//必须这么写为了让m_pStackedWidget在垂直距离的中间

    m_pStackedWidget->addWidget(recordButtonWid);
    m_pStackedWidget->addWidget(ui_2Wid);

    leftMainWidLayout->addWidget(titleLeftWid);
    leftMainWidLayout->addWidget(m_pStackedWidget,0,Qt::AlignCenter);
    leftMainWidLayout->setMargin(0);
    leftMainWid->setLayout(leftMainWidLayout);

    leftMainWid->setFixedWidth(360);

    rightMainWidLayout->addWidget(titleRightWid);
    rightMainWidLayout->addWidget(listWid);
    rightMainWidLayout->setSpacing(0);
    rightMainWidLayout->setMargin(0);
    rightMainWid->setLayout(rightMainWidLayout);

    mainLayout->addWidget(leftMainWid);
    mainLayout->addWidget(rightMainWid);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    mainWid->setLayout(mainLayout);

//    this->setCentralWidget(mainWid);
//    this->setAttribute(Qt::WA_TranslucentBackground);//主窗体透明
//    this->setStyleSheet("border-radius:6px;" );//主窗体圆角(注意：窗体透明与主窗体圆角要搭配使用否则无效)
//    this->show();
//    mainWid->show();
}

void MainWindow::minShow()
{
    mainWid->showMinimized();
}
void MainWindow::maxShow()
{
    if(isMax)
    {
        mainWid->showNormal();
        isMax = false;
    }
    else
    {
        mainWid->showFullScreen();
        isMax = true;
    }

}
void MainWindow::themeWindow(QString themeColor)
{
    if(themeColor == "ukui-dark"||themeColor=="ukui-black")
    {
        menumodule->aboutWindow->setStyleSheet(".QWidget{background-color:rgba(31,32,34,1);}");
        listWid->setStyleSheet("background-color:#141414;"
                               "border-top-right-radius:0px;"
                               "border-bottom-right-radius:0px;");//后期适配主题颜色
        //list->setAttribute(Qt::WA_TranslucentBackground);
        leftMainWid->setStyleSheet("background-color:#222222;"
                                   "border-top-right-radius:0px;"
                                   "border-bottom-right-radius:0px;");
        //主界面
        mainWid->setObjectName("mainWid");//设置命名空间
        mainWid->setStyleSheet(".QWidget{background-color:#141414;}");//自定义窗体(圆角+背景色)
        //mini主界面
        mini.miniWid->setStyleSheet(".Widget{background-color:#222222;}");//后期适配主题颜s;


        //设置界面
        set.mainWid->setObjectName("setMainWid");//设置命名空间
        set.mainWid->setStyleSheet("#setMainWid{background-color:#131314;}");//自定义窗体(圆角+背景色)

        rightMainWid->setStyleSheet(".QWidget{background-color:#141414;\
                                    border-top-left-radius:0px;\
                                    border-bottom-left-radius:0px;}");
        recordButtonWid->setStyleSheet("background-color:#222222;");
//        itemswindow->setStyleSheet("background-color:#222222;");//后期适配主题颜s;

    }
    else
    {
        menumodule->aboutWindow->setStyleSheet(".QWidget{background-color:rgba(255,255,255,1);}");
        listWid->setStyleSheet("background-color:#F8F8F8;"
                               "border-top-right-radius:0px;"
                               "border-bottom-right-radius:0px;");//后期适配主题颜色
        mini.miniWid->setStyleSheet(".QWidget{background-color:#FFFFFF;}");//后期适配主题颜s;

        leftMainWid->setStyleSheet("background-color:#FFFFFF;"
                                   "border-top-right-radius:0px;"
                                   "border-bottom-right-radius:0px;");

        //三联好使了
        rightMainWid->setStyleSheet(".QWidget{background-color:#F8F8F8;\
                                    border-top-left-radius:0px;\
                                    border-bottom-left-radius:0px;}");


        //设置界面
        set.mainWid->setObjectName("setMainWid");//设置命名空间
        set.mainWid->setStyleSheet("#setMainWid{background-color:#FFFFFF;}");//自定义窗体(圆角+背景色)
        //设置界面
        //主界面
        mainWid->setObjectName("mainWid");//设置命名空间
        mainWid->setStyleSheet(".QWidget{background-color:#FFFFFF;}");//自定义窗体(圆角+背景色)
        recordButtonWid->setStyleSheet("background-color:#FFFFFF;opacity:0.1;");
    }
}
void MainWindow::themeButton(QString themeColor)
{
    if(themeColor == "ukui-dark"||themeColor=="ukui-black")
    {
        mini.recordBtn->setStyleSheet("QToolButton{border-radius:12px;}"
                                      "QToolButton::hover{background-color:rgba(255,255,255,0.8);}"
                                      "QToolButton::pressed{background-color:rgba(255,255,255,0.45);}");
        mini.closeBtn->setIcon(QIcon(":/svg/svg/dark-window-close.svg"));
        mini.line->setStyleSheet("border-radius:1px;background-color:#000000;");
        recordButton->setStyleSheet("QPushButton{border-radius:64px;}"
                                    "QPushButton::hover{background-color:rgba(255,255,255,0.8);}"
                                    "QPushButton::pressed{background-color:rgba(255,255,255,0.45);}");
        slider->setStyleSheet("QSlider::handle:horizontal{width:12px;background-color:#E8E8E8;margin:-5px 0px -5px 0px;border-radius:6px;}"
                              "QSlider::groove:horizontal{height:3px;background-color:#656566;}"
                              "QSlider::add-page:horizontal{background-color:#656566;}"
                              "QSlider::sub-page:horizontal{background-color:#E8E8E8;}");
//        list->setAttribute(Qt::WA_TranslucentBackground);
        zeroFile_Messagelb->setStyleSheet("font-size:14px;color:#FFFFFF");
        list->setStyleSheet("QListWidget::item:selected{border:none;}"
                            "QListWidget::item:hover{background-color:#222222;}"
                            "QScrollBar{background-color:#141414;border-radius:2px;width:4px;}"
                            "QScrollBar::add-line:vertical{height: 0px;}"
                            "QScrollBar::sub-line:vertical{height: 0px;}"
                            "QScrollBar::handle:vertical{background-color:rgba(255,255,255,0.2);border-radius:2px;width:4px;min-height:20px;}"
                            );
        //设置按钮
//        setButton->setIcon(QIcon(":/svg/svg/open-menu.svg"));
        setButton->setFixedSize(30,30);
        setButton->setProperty("isWindowButton", 0x1);
        setButton->setProperty("useIconHighlightEffect", 0x2);
        setButton->setFlat(true);
        setButton->setStyleSheet("QPushButton{border-radius:4px;}"
                                 "QPushButton:hover{background-color:rgba(255,255,255,0.1);}"
                                 "QPushButton:pressed{background-color:rgba(255,255,255,0.15);}"
                                 "QPushButton::menu-indicator{image:None;}");

        //最大化按钮
//        maxButton->setIcon(QIcon(":/svg/svg/dark-window-maximize.svg"));
        maxButton->setFixedSize(30,30);
        maxButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.15;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.20;}");
        //关闭按钮
        closeButton->setFixedSize(30,30);
        stopButton->setStyleSheet("QToolButton{image: url(:/svg/svg/finish-defalut-dark.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/finish-hover.svg);}"
                                  "QToolButton:pressed{image: url(:/svg/svg/finish-click.svg);}");

        if(strat_pause)
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-play-dark.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-play-_hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-play-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continu-dark.svg);}"
                                            "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                            "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");

        }
        else
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-dark.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-dark.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");

        }
    }
    else
    {
        mini.recordBtn->setStyleSheet("QToolButton{border-radius:12px;}"
                                      "QToolButton::hover{background-color:rgba(0,0,0,0.15);}"
                                      "QToolButton::pressed{background-color:rgba(0,0,0,0.45);}");

        mini.closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
        mini.line->setStyleSheet("border-radius:1px; background-color:#F2F2F2;");
        recordButton->setStyleSheet("QPushButton{border-radius:64px;}"
                                    "QPushButton::hover{background-color:rgba(0,0,0,0.15);}"
                                    "QPushButton::pressed{background-color:rgba(0,0,0,0.45);}");
        slider->setStyleSheet("QSlider::handle:horizontal{width:12px;background-color:#404040;margin:-5px 0px -5px 0px;border-radius:6px;}"
                              "QSlider::groove:horizontal{height:3px;background-color:rgb(219,219,219);}"
                              "QSlider::add-page:horizontal{background-color:rgb(219,219,219);}"
                              "QSlider::sub-page:horizontal{background-color:#404040;}");
        zeroFile_Messagelb->setStyleSheet("font-size:14px;color:#404040");
        //list->setAttribute(Qt::WA_TranslucentBackground);
        list->setStyleSheet("QListWidget::item:selected{border:none;}"
                            "QListWidget::item:hover{background: rgb(255, 255, 255);}"
                            "QScrollBar{background-color:#F8F8F8;border-radius:2px;width:4px;}"
                            "QScrollBar::add-line:vertical{height: 0px;}"
                            "QScrollBar::sub-line:vertical{height: 0px;}"
                            "QScrollBar::handle:vertical{background-color:#888888;border-radius:2px;width:4px;min-height:20px;}"
                            );
        //设置按钮
//        setButton->setIcon(QIcon(":/svg/svg/open-menu.svg"));
        setButton->setFixedSize(30,30);
        setButton->setStyleSheet("QPushButton{border-radius:4px;}"
                                 "QPushButton:hover{background-color:#E5E5E5;opacity:0.15;}"
                                 "QPushButton:pressed{background-color:#D9D9D9;opacity:0.20;}"
                                 "QPushButton::menu-indicator{image:None;}");




        //最大化按钮
//        maxButton->setIcon(QIcon(":/svg/svg/window-maximize.svg"));
        maxButton->setFixedSize(30,30);
        maxButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.15;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.20;}");


        stopButton->setStyleSheet("QToolButton{image: url(:/svg/svg/finish-defalut-light.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/finish-hover.svg);}"
                                  "QToolButton:pressed{image: url(:/svg/svg/finish-click.svg);}");
        if(strat_pause)
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continue-light.svg);}"
                                         "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                         "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-play-light.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-play-_hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-play-click.svg);}");
        }
        else
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-light.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-light.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");
        }

    }

}
//计算时长
QString MainWindow::playerTotalTime(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileinfo(filePath);
    qint64 fileSize;
    qint64 time;
    QString timeStr;
    if (file.open(QIODevice::ReadOnly))
    {
        if(fileinfo.suffix().contains("mp3"))
        {
            fileSize = file.size();
            qDebug()<<file.size()<<"后缀:mp3余数"<<fileSize%16000;
            time = fileSize/16000;//时间长度=文件大小/每秒字节数
            QTime totalTime(time/3600,(time%3600)/60,time%60);
            timeStr=totalTime.toString("hh:mm:ss");
        }
        else if(fileinfo.suffix().contains("m4a"))
        {
            fileSize = file.size();
            time = fileSize/16000;//时间长度=文件大小/每秒字节数
            QTime totalTime(time/3600,(time%3600)/60,time%60);
            timeStr=totalTime.toString("hh:mm:ss");
            qDebug()<<"文件大小:"<<fileSize<<"时长:"<<timeStr;
        }
        else if(fileinfo.suffix().contains("wav"))
        {
            fileSize = file.size();
            time = fileSize/96000;//时间长度=文件大小/每秒字节数
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
//    if(menumodule->m_pGsettingThemeStatus->get("thememode").toString() == "darkonly")
//    {
//        qDebug()<<menumodule->m_pGsettingThemeStatus->get("thememode").toString();
//        themeStyle("ukui-dark");
//    }
//    else if(menumodule->m_pGsettingThemeStatus->get("thememode").toString() == "lightonly")
//    {
//        qDebug()<<menumodule->m_pGsettingThemeStatus->get("thememode").toString();
//        themeStyle("ukui-white");
//    }
//    else
//    {

        connect(themeData, &QGSettings::changed, this, [=]  ()
        {
            theme = themeData->get("style-name").toString();
            themeStyle(theme);
        });
        themeStyle(themeData->get("style-name").toString());
//    }

}

void MainWindow::updateGsetting_ListWidget()//初始化时配置文件刷新出,传""时为应用开始时的刷新
{
    qDebug() <<"初始化";
    int  m=myThread->readNumList();
    if(m == 1)
    {
        isFileNull(m-1);
    }
    //qDebug()<<m;
    QStringList listRecordPath = myThread->readPathCollected().split(",");
    qDebug()<<listRecordPath;
    QStringList listAmplitude = defaultPathData->get("amplitude").toString().split(";");
    for(int i=1;i<m;i++)
    {
        QString str="";
        str = listRecordPath.at(i);
        qDebug()<<listRecordPath.at(i);
        QFileInfo fileinfo(str);
        QString filesuffix = fileinfo.suffix();//判断文件后缀
        //qDebug()<<fileinfo.isFile();//判断是否为文件，是文件就存在了,因为在本地删除后，同步文件列表下才打开时那个文件也没了
        //qDebug()<<filesuffix;
        QFileInfo fi(str);
        if(fi.exists())
        {
            if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
            {
               slotListItemAdd(str,i);//每当配置文件中有路径时就在list中更新一下
            }
        }
        else
        {
            qDebug()<<str<<"MainWindow:文件或被删除！";
            QString subStr=","+str;//子串
            QString subAmplitudeStr = listAmplitude.at(i-1);
            /*
             * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串内容
             */
            QString oldStr=defaultPathData->get("recorderpath").toString();
            int pos=oldStr.indexOf(subStr);
            QString oldAmplitudeStr = defaultPathData->get("amplitude").toString();
            int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);
            //qDebug()<<pos<<" "<<oldStr;
            //qDebug()<<oldStr.mid(pos,str.length()+1);
            QString newStr = oldStr.remove(pos,str.length()+1);
            myThread->writePathCollected(newStr);
            QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
            defaultPathData->set("amplitude",newAmplitudeStr);

            myThread->writeNumList(myThread->readNumList()-1);
            qDebug()<<myThread->readPathCollected();
        }

    }
    isFirstRun = false;//所有文件都显示全才置为false;

}

void MainWindow::themeStyle(QString themeColor)
{
    qDebug()<<themeColor;
    themeButton(themeColor);//控件类切换
    themeWindow(themeColor);//窗体类切换
}

void MainWindow::checkSingle(QStringList path)//检查单例模式
{
    QString str;
    if(path.size() > 1)
    {
        str = path[1];
    }

    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    //兼容VNC的单例模式
    QString lockPath = homePath.at(0) + "/.config/kylin-recorder-lock";
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        exit(1);
    }
    if (lockf(fd, F_TLOCK, 0)) {

         QDBusInterface interface( "org.ukui.kylin_recorder", "/org/ukui/kylin_recorder","org.ukui.kylin_recorder",
                                   QDBusConnection::sessionBus());
         if(path.size() == 1)
             interface.call( "command_Control", str);
         if(path.size() == 2)
             interface.call( "command_Control", str);

        qDebug()<<"Can't lock single file, kylin-recorder is already running!";
        exit(0);

    }
    isFirstObject = true;//我是首个对象
    argName << str;
    qDebug()<<"argName:"<<argName<<"str:"<<str<<"path:"<<path;

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
    if(this->isPress)//鼠标按压时移动
    {
        this->move(this->winPos - (this->dragPos - event->globalPos()));
        this->setCursor(Qt::ClosedHandCursor);
        event->accept();
    }
}

void MainWindow::miniShow()
{
    mini.miniWid->showNormal();
    mainWid->hide();
}

//开始和暂停
void MainWindow::play_pause_clicked()
{
    static QTime pauseTime;
    if(strat_pause)
    {
        emit playRecord();
        set.radioButton_6->setEnabled(false);
        set.radioButton_7->setEnabled(false);
        set.radioButton_8->setEnabled(false);
//        menumodule -> menuButton->setEnabled(false);
        isRecording = true;//开始时正在录音的标记值为true,其为true时禁止Item的悬浮特效
        cut = QTime::currentTime();//记录开始时的时间
        int t = pauseTime.secsTo(cut);//点击暂停时时间与点击恢复计时的时间差值
        limitTimer->start(1000);
        baseTime = baseTime.addSecs(t);
        pTimer->start(100);
        mini.baseTime = mini.baseTime.addSecs(t);
        mini.pTimer->start(100);
        mini.start_pause = false;
        strat_pause = false;
        if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-dark.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-dark.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        }
        else
        {
            //点击完之后要修改悬停样式。
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-light.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-light.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        }
    }
    else
    {
        qDebug()<<"pause录音";
        emit pauseRecord();
        isRecording = false;//暂停时正在录音的标记值为false,其为false时Item的悬浮特效可以被开启
        limitTimer->stop();
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
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-play-dark.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-play-_hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-play-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continu-dark.svg);}"
                                            "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                            "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
        }
        else
        {
            mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-play-light.svg);}"
                                               "QToolButton:hover{image:url(:/svg/svg/mini-play-_hover.svg);}"
                                               "QToolButton:pressed{image:url(:/svg/svg/mini-play-click.svg);}");
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continue-light.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
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
        isRecording = false;//停止录音时此值为false,其为false时Item的悬浮特效可以被开启
        set.radioButton_6->setEnabled(true);
        set.radioButton_7->setEnabled(true);
        set.radioButton_8->setEnabled(true);
//        menumodule->menuButton->setEnabled(true);
        limitTimer->stop();//停止记录录音时间
        pTimer->stop();//计时停止
        mini.pTimer->stop();
        emit stopRecord();
        //停止之后把数组清0
        for(int i = 0;i<INIT_MAINWINDOW_RECTANGLE_COUNT;i++)
        {
            valueArray[i] = 0;
        }
        if(strat_pause)
        {
            strat_pause = false;
            mini.start_pause = false;
        }
        //停止按钮触发后的界面
        play_pauseButton->setEnabled(false);
        if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continu-dark.svg);}"
                                            "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                            "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");

        }
        else
        {
            play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continue-light.svg);}"
                        "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                        "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
        }

        showTimelb->setText("00:00:00");//主界面计时停止
        mini.timelb->setText("00:00:00");//mini模式计时停止
        //showTimelb->setStyleSheet("font: bold; font-size:20px;");
        stop=false;
        m_pStackedWidget->setCurrentIndex(0);
        mini.recordStackedWidget->setCurrentIndex(0);
        timeTag = 0;//只有在停止时让计时归零
    }

}

//定时器启动时调用此函数
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
//    qDebug()<<"秒："<<showTime.second();
    timeTag = showTime.minute();
    this->showTimelb->setText(timeStr);
}

void MainWindow::limitRecordingTime()
{
    qDebug()<<"查看计数:"<<timeTag;
    if(timeTag >= 60)//超过15分钟自动保存
    {
       stop_clicked();
    }
}

void MainWindow::mainWindow_page2()
{
    maxNum.clear();
    for(int i=0;i<rectangleCount;i++)maxNum.append(0);

    slider->setOrientation(Qt::Horizontal);
    slider->setValue(myThread->soundVolume);
    voiceBtn->setProperty("isWindowButton", 0x1);
    voiceBtn->setProperty("useIconHighlightEffect", 0x2);
    voiceBtn->setIconSize(QSize(24,24));//重置图标大小
    voiceBtn->setFixedSize(22,18);
    voiceBtn->setAutoRaise(true);
    play_pauseButton->setIconSize(QSize(56,56));//重置图标大小
    play_pauseButton->setEnabled(true);//按下后，开始录音可以按暂停
    if(themeData->get("style-name").toString() == "ukui-dark"||themeData->get("style-name").toString() == "ukui-black")
    {
        mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-dark.svg);}"
                       "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                       "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");
        play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-dark.svg);}"
                    "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                    "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        stopButton->setStyleSheet("QToolButton{image: url(:/svg/svg/finish-defalut-dark.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/finish-hover.svg);}"
                                  "QToolButton:pressed{image: url(:/svg/svg/finish-click.svg);}");

    }
    else
    {
        mini.start_pauseBtn->setStyleSheet("QToolButton{border-radius:12px;image:url(:/svg/svg/mini-suspend-light.svg);}"
                       "QToolButton:hover{image:url(:/svg/svg/mini-suspend-hover.svg);}"
                       "QToolButton:pressed{image:url(:/svg/svg/mini-suspend-click.svg);}");

        play_pauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-light.svg);}"
                                        "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                                        "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        stopButton->setStyleSheet("QToolButton{image: url(:/svg/svg/finish-defalut-light.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/finish-hover.svg);}"
                                  "QToolButton:pressed{image: url(:/svg/svg/finish-click.svg);}");
    }


    stopButton->setIconSize(QSize(56,56));//重置图标大小
    showTimelb->setText("00:00:00");
    showTimelb->setStyleSheet("font: bold; font-size:18px;");


    showTimelbLayout->addWidget(showTimelb,0,Qt::AlignCenter);//在布局的中央
    showTimelbWid->setLayout(showTimelbLayout);
    monitorWaveLayout=new QHBoxLayout();//波形图所在的布局
    for (int i=0;i<rectangleCount;i++)//频率直方图
    {
        myWave *wave=new myWave(this);//每次都要初始化一个矩形框
//        wave=new myWave(this);
//      wave->setMaximumWidth(100);
        wave->setRange(0,100);
        mywave.push_back(wave);
        monitorWaveLayout->addWidget(wave);

    }
    monitorWaveLayout->setSpacing(2);
    waveWid->setLayout(monitorWaveLayout);
    waveWid->setFixedHeight(120);

    voiceAndSliderLayout->addWidget(voiceBtn);
    voiceAndSliderLayout->addWidget(slider);
    voiceAndSliderWid->setLayout(voiceAndSliderLayout);

    controlPlay_PauseLayout->addWidget(stopButton);
    controlPlay_PauseLayout->addWidget(play_pauseButton);
    controlPlay_PauseWid->setLayout(controlPlay_PauseLayout);

    ui_2Layout->addWidget(showTimelbWid);
    ui_2Layout->addWidget(waveWid);
//    ui_2Layout->addWidget(voiceAndSliderWid);
    ui_2Layout->addWidget(controlPlay_PauseWid);

    ui_2Wid->setLayout(ui_2Layout);
}

void MainWindow::getPlayingSlot(bool playingState)
{
    isplaying = playingState;
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
    if(!isplaying)//判断是否有音频在播放，若无音频播放则可以录音
    {
        stop=true;//stop按钮默认是true代表停止中
        isRecording = true;//正在录音时此标记为true，此为true时悬浮特效被禁止,这一行一定要在前面
        m_pStackedWidget->setCurrentIndex(1);
        mini.recordStackedWidget->setCurrentIndex(1);//切换至录音按钮
        emit startRecord();
        mainWindow_page2();//必须加
        //刚开始点击按钮时才可以开启定时器
        limitTimer->start(1000);//开始记录录音时长
        pTimer->start(100);
        baseTime = baseTime.currentTime();
        mini.baseTime = mini.baseTime.currentTime();
        mini.pTimer->start(100);
        set.radioButton_6->setEnabled(false);
        set.radioButton_7->setEnabled(false);
        set.radioButton_8->setEnabled(false);
//        menumodule->menuButton->setEnabled(false);
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

void MainWindow::changeVoicePicture()
{
    if(slider->value()== 0)
    {
        voiceBtn->setIcon(QIcon::fromTheme("audio-volume-muted-symbolic"));
    }
    else if(slider->value() <= 30 && slider->value() > 0)
    {
        voiceBtn->setIcon(QIcon::fromTheme("audio-volume-low"));
    }
    else if(slider->value() > 30 && slider->value() <=70)
    {
        voiceBtn->setIcon(QIcon::fromTheme("audio-volume-medium"));
    }
    else if(slider->value() > 70 && slider->value() <=100)
    {
        voiceBtn->setIcon(QIcon::fromTheme("audio-volume-high"));
    }
}

void MainWindow::goset()
{
    set.mainWid->show();
    set.mainWid->move(mainWid->geometry().center() - set.mainWid->rect().center());
}

void MainWindow::slotListItemAdd(QString fileName,int i)
{
    qDebug()<<"更新";
    itemswindow = new ItemsWindow(this);//初始化Item录音文件类必须加this,
                                        //因为后期要判断子类的子控件
//    ItemsWindow *itemswindow = new ItemsWindow(this);
    itemswindow->listNum->setText(tr("recorder")+QString::number(i));
    //添加当前录音文件的文件名(以时间命名)
    itemswindow->recordFileName->setText(fileName.split("/").last());
    //添加当前录音文件的时长,要判断一下是按了停止按钮还是应用重启时的刷新
    if(isFirstRun){
        //首次
        itemswindow->timelengthlb->setText(playerTotalTime(fileName));
    }else{
        //非首次:根据showTimelb设置timelengthlb显示的内容
        itemswindow->timelengthlb->setText(showTimelb->text());
    }
    QListWidgetItem *aItem = new QListWidgetItem(list);//添加自定义的item
    list->setItemWidget(aItem,itemswindow->clipperstackWid);
    //list->addItem(aItem);
    aItem->setSizeHint(QSize(0,60));
    isFileNull(list->count());//item个数从0开始增加时文件列表不应该显示"文件列表空"的字样

}



bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{

    if(obj == list)
    {
        if(event->type()==QEvent::Wheel)
        {
            QWheelEvent *wheelEvent=static_cast<QWheelEvent *>(event);
            if(wheelEvent->delta()>0)
            {
                //实现1，这里你可以填写自己想要实现的效果
                qDebug()<<"上";
            }
            else
            {
                //实现2，这里你可以填写自己想要实现的效果
                qDebug()<<"下";
            }
        }


    }
    else if(obj == mainWid)
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

void MainWindow::wheelEvent(QWheelEvent *wheel)
{
//    if(wheel->delta()>0)
//    {
//        qDebug()<<"上";
//    }
//    else
//    {
//        qDebug()<<"下";
//    }
}




