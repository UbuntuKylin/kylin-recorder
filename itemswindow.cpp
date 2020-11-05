#include "itemswindow.h"
#include "mainwindow.h"
ItemsWindow::ItemsWindow(QWidget *parent) : QMainWindow(parent)
{
    setItemWid();
}
void ItemsWindow::setItemWid()
{

    playList = new QMediaPlaylist;//播放列表
    player = new QMediaPlayer;//媒体文件
    playSlider=new QSlider(this);




    playSlider->installEventFilter(this);//安装事件过滤器
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

    connect(playSlider,&QSlider::valueChanged,this,&ItemsWindow::setPosition);
    connect(playSlider,SIGNAL(sliderPressed()),this,SLOT(slidePress()));
    connect(playSlider,SIGNAL(sliderReleased()),this,SLOT(slideRelease()));
    playSlider->setOrientation(Qt::Horizontal);
    playSlider->setStyleSheet("QSlider::handle:horizontal{width:2px;background-color:#F94A76;margin:-5px 0px -5px 0px;border-radius:1px;}"
                              "QSlider::groove:horizontal{height:3px;background-color:#EDEDED;}"
                              "QSlider::add-page:horizontal{background-color:#EDEDED;}"
                              "QSlider::sub-page:horizontal{background-color:#F94A76;}");
    playSlider->setFixedHeight(3);//后期要改成3
    //        playSlider->setStyleSheet("background-color:purple;");
    recordFileNamelb=new QLabel(this);
    dateTimelb=new QLabel(this);
    dateTimelb->setObjectName("dateTimelb");
    testlb=new QLabel(this);
    testlb->setText(" ");
    timelengthlb=new QLabel(this);
    timelengthlb->setText("00:00:00");
    itemPlay_PauseButton=new QToolButton(this);
//    itemPlay_PauseButton->setIcon(QIcon(":/svg/svg/bofang.svg"));
    itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);}"
                          "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}");
    //"QToolButton:pressed{background-color:#F96E92;opacity:0;border-radius:16px}"
    itemPlay_PauseButton->setFixedSize(32,32);
    clipButton=new QToolButton(this);
    clipButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bianji.svg);}"
                              "QToolButton:hover{image: url(:/svg/svg/jianji_select.svg);}");
    clipButton->setFixedSize(32,32);

    deleteButton=new QToolButton(this);
    deleteButton->setStyleSheet("QToolButton{image: url(:/svg/svg/delete.svg);}"
                                "QToolButton:hover{image: url(:/svg/svg/delete_select.svg);}");
    deleteButton->setFixedSize(32,32);


    itemsWid = new QWidget(this);
    itemsWid->installEventFilter(this);

    itemTopWid = new QWidget(this);
    itemBottomWid = new QWidget(this);
    fileName_dateTimeWid = new QWidget(this);
    threeButtonWid = new QWidget(this);
    firstWid=new QWidget(this);
    stackWid=new QWidget(this);

    //剪裁布局start

    line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border-radius: 3px; background-color:red;");
    timelengthlb2=new QLabel(this);
    timelengthlb2->setText("00:00:00");

    cancelButton=new QToolButton(this);
    cancelButton->setText("取消");
    cancelButton->setFixedSize(40,25);

    finishButton=new QToolButton(this);
    finishButton->setText("完成");
    finishButton->setFixedSize(40,25);
    clipperstackWid=new QWidget(this);
    clipperWid = new QWidget(this);//剪裁的总Wid
    bottomWid = new QWidget(this);//底部Wid


    bottomLayout = new QHBoxLayout(this);//底部布局
    bottomLayout->addWidget(timelengthlb2);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(finishButton);
    bottomWid->setLayout(bottomLayout);
    clipperLayout = new QVBoxLayout(this);//剪裁布局
    clipperLayout->addWidget(line);
    clipperLayout->addWidget(bottomWid);
    clipperWid->setLayout(clipperLayout);



    //剪裁布局end





    fileName_dateTimeLayout = new QVBoxLayout();
    fileName_dateTimeLayout->addWidget(recordFileNamelb);
    fileName_dateTimeLayout->addWidget(dateTimelb);
    fileName_dateTimeWid->setLayout(fileName_dateTimeLayout);

    timeLengthLayout=new QHBoxLayout();
    timeLengthLayout->addWidget(timelengthlb,0,Qt::AlignCenter);
    firstWid->setLayout(timeLengthLayout);

    threeButtonLayout = new QHBoxLayout();
    threeButtonLayout->addWidget(itemPlay_PauseButton);
    //threeButtonLayout->addWidget(clipButton);//2020.11.4 隐藏此功能后续开发。。。
    threeButtonLayout->addWidget(deleteButton);
    threeButtonWid->setLayout(threeButtonLayout);



    stackLayout=new QStackedLayout();//堆叠布局
    stackLayout->addWidget(firstWid);
    stackLayout->addWidget(threeButtonWid);
    stackWid->setLayout(stackLayout);

    itemBottomLayout=new QHBoxLayout();
    itemBottomLayout->addWidget(fileName_dateTimeWid);
    itemBottomLayout->addWidget(stackWid,0,Qt::AlignRight);
    itemBottomLayout->setSpacing(0);
    itemBottomLayout->setMargin(0);
    itemBottomWid->setLayout(itemBottomLayout);

    recordFileNamelb->setStyleSheet("font-size:14px;");
    dateTimelb->setStyleSheet("font-size:14px;");



    itemLayout=new QVBoxLayout();//列表项布局
    itemLayout->addWidget(playSlider);
    itemLayout->addWidget(itemBottomWid);
    itemLayout->setMargin(0);
    itemLayout->setSpacing(0);
    itemsWid->setLayout(itemLayout);



    connect(itemPlay_PauseButton,&QToolButton::clicked,this,&ItemsWindow::itemPlay_PauseClicked);
    connect(clipButton,&QToolButton::clicked,this,&ItemsWindow::clipper);
    connect(deleteButton,&QToolButton::clicked,this,&ItemsWindow::delFile);

    //setStyleSheet("border-radius:0px;");
    //fileName_dateTimeWid->setStyleSheet("background-color:blue;");
    this->setStyleSheet("border-radius:6px;background-color:#FBEFED;");//自定义窗体(圆角+背景色)
}
void ItemsWindow::positionChange(qint64 position)
{
    if(player->state()==QMediaPlayer::PlayingState)
    {
        playSlider->setValue(static_cast<int>(position));
        QTime duration(0, static_cast<int>(position) / 60000, static_cast<int>((position % 60000) / 1000.0));
        QString str_time = duration.toString("mm:ss");
        QTime totalTime(0,(player->duration()/60000) % 60,
                       (player->duration() / 1000) % 60);
        qDebug()<<str_time + "/" + totalTime.toString("mm:ss");
//        if (position > player->duration()) {
//            qDebug()<<"列表播放结束 停止" ;
//            player->stop();
//        }
        if(str_time==totalTime.toString("mm:ss")&&str_time!="00:00")
        {
            qDebug()<<"列表播放结束 停止" ;
            player->stop();
            itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);}"
                                                "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}");
            play_pause = false;
            setPosition(position);
        }
    }
}
void ItemsWindow::durationChange(qint64 duration)   //更新播放进度
{
    playSlider->setRange(0,static_cast<int>(duration));
    playSlider->setEnabled(duration>0);
    playSlider->setPageStep(static_cast<int>(duration)/10);
}
void ItemsWindow::setPosition(int position)
{
    if (qAbs(player->position() - position) > 99)
        player->setPosition(position);
}
bool ItemsWindow::eventFilter(QObject *obj, QEvent *event)   //鼠标滑块点击
{
    if(obj == playSlider)
    {
        if (event->type() == QEvent::MouseButtonPress)           //判断类型
        {
            //qDebug()<<"左键";
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) //判断左键
            {
               int dur = playSlider->maximum() - playSlider->minimum();
               int pos = playSlider->minimum() + dur * ((double)mouseEvent->x() / playSlider->width());
               if(pos != playSlider->sliderPosition())
               {
                   playSlider->setValue(pos);
               }
            }
        }
    }
    else if(obj==itemsWid)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) //判断左键
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                int nIndex = stackLayout->currentIndex();
                //qDebug()<<MainWindow::mutual->list->currentRow();
                // 获取下一个需要显示的页面索引
                nIndex++;
                // 当需要显示的页面索引大于等于总页面时，切换至首页s
                stackLayout->setCurrentIndex(nIndex%2);//切换至录音按钮stackLayout
            }
        }

    }


    return QObject::eventFilter(obj,event);
}
void ItemsWindow::slidePress() //滑动条鼠标按下
{
     player->pause();

}
void ItemsWindow::slideRelease()   //滑动条鼠标弹起
{
    player->play();
}
//开始播放和暂停播放
void ItemsWindow::itemPlay_PauseClicked()
{
    MyThread *myth=new MyThread();
    QLabel *label =  itemsWid->findChild<QLabel *>(dateTimelb->objectName());
    myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath=myth->readPathCollected().split(",");
    int m=myth->readNumList()-1;
    if(play_pause)
    {
        for(int i=1;i<=m;i++)
        {
            QString str=listRecordPath.at(i);
            if(str.contains(label->text()))
            {
                QFileInfo fi(str);
                if(fi.exists())
                {
                    player->pause();
                    qDebug()<<"存在暂停"<<label->text();

                }
                else
                {
                    WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告")
                                             ,tr("文件路径不存在,或被删除!"),QMessageBox::Yes );
                    WrrMsg->button(QMessageBox::Yes)->setText("确 定");
                    WrrMsg->exec();
                    return ;
                }
            }
        }
        itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);border-radius: 16px; }"
                              "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}"
                              "QToolButton:pressed{background-color:#F96E92;}");
        play_pause=false;
    }
    else
    {
        for(int i=1;i<=m;i++)
        {
            QString str=listRecordPath.at(i);
            if(str.contains(label->text()))
            {
                QFileInfo fi(str);
                if(fi.exists())
                {
                    emit judgePlay(MainWindow::mutual->list->currentRow());
                    player->setMedia(QUrl::fromLocalFile(str));
                    player->setVolume(50);
                    player->play();
                    qDebug()<<"存在播放"<<label->text();
                }
                else
                {
                    WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告")
                                             ,tr("文件路径不存在,或被删除!"),QMessageBox::Yes );
                    WrrMsg->button(QMessageBox::Yes)->setText("确 定");
                    WrrMsg->exec();
                    return ;
                }
            }
        }
        itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/zanting.svg);border-radius: 16px; }"
                              "QToolButton:hover{image: url(:/svg/svg/zanting_select.svg);}"
                              "QToolButton:pressed{background-color:#F96E92;}");

        play_pause = true;
    }

}
void ItemsWindow::delFile()
{
    MyThread *myth=new MyThread();//构造函数实例化后构造函数被调用。recordPathData在MyThread的构造里面
    QLabel *label =  itemsWid->findChild<QLabel *>(dateTimelb->objectName());
    myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath=myth->readPathCollected().split(",");
    int m=myth->readNumList()-1;//因为配置文件初始为1
    if(m<=0)
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告")
                                 ,tr("当前列表文件数目为0"),QMessageBox::Yes );
        WrrMsg->button(QMessageBox::Yes)->setText("确 定");
        WrrMsg->exec();
        return ;
    }
    for(int i=1;i<=m;i++)
    {
        QString str=listRecordPath.at(i);
        if(str.contains(label->text()))
        {
            QFileInfo fi(str);
            if(fi.exists())
            {
                QFileInfo fileinfo(str);
                QString filesuffix = fileinfo.suffix();//判断文件后缀
                if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")))
                {
                    qDebug()<<"文件存在!可以删除";
                    if(player->state()==QMediaPlayer::PlayingState)
                    {
                        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告")
                                                 ,tr("正在播放，请停止后再删除!"),QMessageBox::Yes );
                        WrrMsg->button(QMessageBox::Yes)->setText("确 定");
                        WrrMsg->exec();
                        return ;
                    }
                    QString subStr=","+str;//子串
                    /*
                     * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串内容
                    */
                    QString oldStr=myth->recordData->get("recorderpath").toString();
                    int pos=oldStr.indexOf(subStr);
                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    myth->writePathCollected(newStr);
                    myth->writeNumList(myth->readNumList()-1);
                    //根据索引值删除listwidget列表的Item，要注意配置文件的更新以及本地文件的删除
                    int index=MainWindow::mutual->list->currentRow();
                    MainWindow::mutual->list->takeItem(index);//删除操作
                    QFile::remove(str);//从本地删除
                    qDebug()<<myth->readPathCollected();//删除操作后，更新输出
                }
                else
                {
                    qDebug()<<"文件不存在!或已经被删除!";
                }
            }
            else
            {
                WrrMsg = new QMessageBox(QMessageBox::Warning,tr("警告")
                                         ,tr("文件路径不存在,或被删除!"),QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText("确 定");
                WrrMsg->exec();
                return ;
            }

        }
    }
}
void ItemsWindow::clipper()
{
    qDebug()<<"剪裁！";
    int nIndex = clipperstackLayout->currentIndex();
    // 获取下一个需要显示的页面索引
    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页s
   clipperstackLayout->setCurrentIndex(nIndex%2);//切换至录音按钮stackLayout
}





