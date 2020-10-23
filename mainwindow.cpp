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
#define rectangleCount 40//矩形条个数
MainWindow *MainWindow::mutual=nullptr;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    checkSingle();//检查单例模式
    mutual=this;
    int WIDTH=360;
    int HEIGHT=300;
    this->resize(WIDTH,HEIGHT);
    this->setWindowIcon(QIcon(":/svg/svg/recording_128.svg"));
    //屏幕中间
    this->move((QApplication::desktop()->width() -WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);

    lb=new QLabel(this);
    piclb=new QLabel(this);//窗体左上角图片Label
    setButton=new QToolButton(this);
    max_minButton=new QToolButton(this);
    closeButton= new QToolButton(this);
    recordButton=new QToolButton(this);

    pTimer = new QTimer;//第二个页面的控件初始化
    showTimelb=new QLabel(this);
    voicelb=new QLabel(this);
    slider =new QSlider(this);
    stopButton= new QToolButton(this);
    play_pauseButton= new QToolButton(this);

    ui_2Wid=new QWidget(this);//第二个页面布局
    timeWid=new QWidget(this);
    waveWid=new QWidget(this);//波形图
    voiceWid=new QWidget(this);
    playWid=new QWidget(this);

    layout1=new QHBoxLayout(this);

    layout3=new QHBoxLayout(this);
    layout4=new QHBoxLayout(this);
    ui_2Layout =new QVBoxLayout(this);
    //线程的使用
    myThread=new MyThread;//子线程
    thread=new QThread(this);//主线程

    connect(this,&MainWindow::startRecord,myThread,&MyThread::record_pressed);
    connect(myThread,&MyThread::recordPaint,this,&MainWindow::recordPaint);
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


    //点击关闭时把线程关掉
    connect(closeButton,&QToolButton::clicked,[=](){
        thread->quit();
          thread->wait();
    });
    setWindowFlags((Qt::FramelessWindowHint));//设置窗体无边框
    setWindowTitle("麒麟录音");


    piclb->setStyleSheet("QLabel{border-image: url(:/png/png/recording_32.png);}");
    piclb->setFixedSize(25,25);
    lb->setText("麒麟录音");//？字体待修改

//    recordButton->setFlat(true);//就是这句能够实现按钮透明，用png图片时很有用
//    recordButton->setStyleSheet("border: 0px");//消除边框，取消点击效果
//    QIcon myicon; //新建QIcon对象
//    myicon.addFile(tr(":/png/png/recording_128.png")); //让QIcon对象指向想要的图标
//    recordButton->setIcon(myicon); //给按钮添加图标
    recordButton->setIconSize(QSize(130,130));//重置图标大小
    recordButton->setStyleSheet("QToolButton{image: url(:/png/png/recording_128.png}");
    recordButton->setStyleSheet("background-color:transparent");

    //设置按钮
    setButton->setIcon(QIcon(":/png/png/set.png"));
    setButton->setFixedSize(30,30);

    setButton->setStyleSheet("QToolButton{border-radius:4px;}"
                             "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                             "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
    setButton->hide();//********************2020.9.24当前设置不可点击。
    //最大最小化按钮
    max_minButton->setIcon(QIcon(":/png/png/mini.png"));
    max_minButton->setFixedSize(30,30);
    max_minButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                 "QToolButton:hover{background-color:#E5E5E5;opacity:0.1;}"
                                 "QToolButton:pressed{background-color:#D9D9D9;opacity:0.15;}");
    //关闭按钮
    closeButton->setIcon(QIcon(":/png/png/close.png"));
    closeButton->setFixedSize(30,30);
    closeButton->setStyleSheet("QToolButton{border-radius:4px;}"
                               "QToolButton:hover{background-color:#F04234}"
                               "QToolButton:pressed{background-color:#D83436}");

    recordButton->setIcon(QIcon(":/png/png/recording_128.png"));
    recordButton->setStyleSheet("border: 0px");//消除边框，取消点击效果

    connect(max_minButton, &QToolButton::clicked,this,&MainWindow::miniShow);
    connect(closeButton,&QToolButton::clicked,this,&MainWindow::close);
    connect(recordButton, &QPushButton::clicked, this, &MainWindow::switchPage);
    connect(setButton, &QToolButton::clicked, this, &MainWindow::goset);


    mainWid = new QWidget(this);
    titleWid = new QWidget(this);
    recordButtonWid = new QWidget(this);

    titleLayout= new QHBoxLayout(this);
    titleLayout->addWidget(piclb);
    titleLayout->addWidget(lb);
    titleLayout->addWidget(setButton);
    titleLayout->addWidget(max_minButton);
    titleLayout->addWidget(closeButton);
    titleLayout->setSpacing(1);
    titleLayout->setMargin(0);
    titleWid->setLayout(titleLayout);


    m_pStackedLayout=new QStackedLayout();//堆栈布局
    m_pStackedLayout->addWidget(recordButton);
    recordButtonWid->setLayout(m_pStackedLayout);

    mainLayout= new QVBoxLayout(this);
    mainLayout->addWidget(titleWid);
    mainLayout->addWidget(recordButtonWid);
    mainWid->setLayout(mainLayout);

    this->setCentralWidget(mainWid);
    this->setAttribute(Qt::WA_TranslucentBackground);//主窗体透明
    this->setStyleSheet("border-radius:12px;");
    mainWid->setStyleSheet("border-radius:12px;background-color:#FFFFFF;");//自定义窗体(圆角+背景色)

    maxNum.clear();
    for(int i=0;i<rectangleCount;i++)maxNum.append(5);
}
void MainWindow::checkSingle()//检查单例模式
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    //qDebug()<<homePath;
    QString lockPath = homePath.at(0) + "/.config/kylin-recorder-lock";
    qDebug()<<lockPath;
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd < 0) { exit(1); }

    if (lockf(fd, F_TLOCK, 0)) {
        qDebug()<<"Can't lock single file, kylin-recorder is already running!";
        exit(0);
    }
}
MainWindow::~MainWindow()
{

}
void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        this->isPress = true;
        this->winPos = this->pos();
        this->dragPos = event->globalPos();
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    this->isPress = false;
    this->setCursor(Qt::ArrowCursor);

}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
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
       QTime cut = QTime::currentTime();//记录开始时的时间
       int t = pauseTime.secsTo(cut);//点击暂停时时间与点击恢复计时的时间差值
       baseTime = baseTime.addSecs(t);
       pTimer->start(1);
       mini.baseTime=mini.baseTime.addSecs(t);
       mini.pTimer->start(1);
       mini.start_pause=false;
       strat_pause=false;
       mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/pause_mini.png);}"
                                          "QToolButton:hover{image:url(:/png/png/pause_mini_hover.png);}"
                                          "QToolButton:pressed{image:url(:/png/png/pause_mini_click.png);}");
       //点击完之后要修改悬停样式。
       play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/pause.png);}"
                   "QToolButton:hover{image: url(:/png/png/pause_hover.png);}"
                   "QToolButton:pressed{image: url(:/png/png/pause_click.png);}");
    }
    else
    {
        qDebug()<<"pause";
        emit pauseRecord();
        pTimer->stop();
        mini.pTimer->stop();
        pauseTime = QTime::currentTime();//记录一下当前暂停时的时间
        mini.start_pause = true;
        strat_pause = true;
        //点击完之后要修改悬停样式。
        mini.start_pauseBtn->setStyleSheet("QToolButton{image:url(:/png/png/start_mini.png);}"
                                          "QToolButton:hover{image:url(:/png/png/start_mini_hover.png);}"
                                          "QToolButton:pressed{image:url(:/png/png/start_mini_click.png);}");
        play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/start.png);}"
                    "QToolButton:hover{image: url(:/png/png/start_hover.png);}"
                    "QToolButton:pressed{image: url(:/png/png/start_click.png);}");

    }

}

void MainWindow::recordPaint(int value)
{
    //qDebug()<<"=========="<<value;
    maxNum.prepend(value/300);//将元素插入到Vector的开始
    maxNum=maxNum.mid(0,rectangleCount);
    for(int i=0;i<rectangleCount;i++)
     mywave.at(i)->setValue(maxNum.at(i));
}

void MainWindow::stop_clicked()//停止按钮
{
    if(stop)
    {
        pTimer->stop();//计时停止
        mini.pTimer->stop();
        emit stopRecord();
        if(strat_pause)
        {
            strat_pause=false;
        }
        //停止按钮触发后的界面
        play_pauseButton->setEnabled(false);
        play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/start.png);}"
                    "QToolButton:hover{image: url(:/png/png/start_hover.png);}"
                    "QToolButton:pressed{image: url(:/png/png/start_click.png);}");
        showTimelb->setText("00:00:00");
        mini.timelb->setText("00:00:00");
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
    for(int i=0;i<rectangleCount;i++)maxNum.append(5);
    emit startRecord();
    slider->setValue(50);//初始音量为50
    voicelb->setStyleSheet("QLabel{image: url(:/png/png/voice.png);}");//初始图标为有声图标
    stopButton->setStyleSheet("QToolButton{image: url(:/png/png/stop.png);}"
                              "QToolButton:hover{image: url(:/png/png/stop_hover.png);}"
                              "QToolButton:pressed{image: url(:/png/png/stop_click.png);}");

    play_pauseButton->setIconSize(QSize(56,56));//重置图标大小
    play_pauseButton->setEnabled(true);//按下后，开始录音可以按暂停
    play_pauseButton->setStyleSheet("QToolButton{image: url(:/png/png/pause.png);}"
                "QToolButton:hover{image: url(:/png/png/pause_hover.png);}"
                "QToolButton:pressed{image: url(:/png/png/pause_click.png);}"                    );
    baseTime = baseTime.currentTime();
    pTimer->start(1);
    mini.baseTime=mini.baseTime.currentTime();
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
    layout2=new QHBoxLayout(this);//波形图所在的布局
    for (int i=0;i<rectangleCount;i++)//频率直方图
    {
        myWave *wave=new myWave(this);//每次都要初始化一个矩形框
//      wave->setMaximumWidth(100);
        wave->setRange(0,100);
        mywave.push_back(wave);
        layout2->addWidget(wave);
    }
    waveWid->setLayout(layout2);
    waveWid->setFixedHeight(70);



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
    for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        if(deviceInfo.deviceName().contains("auto_null"))
        {
            WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告"),tr("没检测到输入设备！"),QMessageBox::Yes );
            WrrMsg->button(QMessageBox::Yes)->setText("确 定");
            WrrMsg->exec();
            return;
        }
        if(deviceInfo.deviceName().contains("alsa_input"))
        {
            qDebug()<<"录音设备是:"<<deviceInfo.deviceName();
            break;
        }
    }
    mainWindow_page2();
    int nCount = m_pStackedLayout->count();
    int nIndex = m_pStackedLayout->currentIndex();
    // 获取下一个需要显示的页面索引
    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;
    mini.recordStackedLayout->setCurrentIndex(nIndex);
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
//    w2=new two(this);
    set.show();
}
//保存名字
void MainWindow::setuserdirectory()
{
    //daultfileName=QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    qDebug()<< "desktop_path:" << desktop_path;
}
