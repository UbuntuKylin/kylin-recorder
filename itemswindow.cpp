#include "itemswindow.h"
#include "mainwindow.h"
ItemsWindow::ItemsWindow(QWidget *parent) : QMainWindow(parent)
{
    itemData = new QGSettings(KYLINRECORDER);
    darkData = new QGSettings(FITTHEMEWINDOW);
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
    recordFileNamelb->setStyleSheet("font-size:14px;");

    dateTimelb=new QLabel(this);
    dateTimelb->setObjectName("dateTimelb");//设置对象名
    dateTimelb->setStyleSheet("font-size:14px;");
    //dateTimelb->setFixedHeight(12);

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


    itemsWid = new QWidget();


    itemsWid->installEventFilter(this);

    itemTopWid = new QWidget();
    itemBottomWid = new QWidget();
    fileName_dateTimeWid = new QWidget();
    threeButtonWid = new QWidget();
    firstWid=new QWidget();
    stackWid=new QWidget();
    clipperstackWid=new QStackedWidget();//裁剪的堆叠Wid
    splitLinestackWid=new QStackedWidget();//分隔线布局

    line = new QFrame();//分割线
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border-radius: 6px; background-color:#EFE4E1;");

    clipperFun();//剪裁布局

    fileName_dateTimeLayout = new QVBoxLayout();
    fileName_dateTimeLayout->addWidget(recordFileNamelb);
    fileName_dateTimeLayout->addWidget(dateTimelb);
    fileName_dateTimeLayout->setSpacing(0);
    fileName_dateTimeWid->setLayout(fileName_dateTimeLayout);

    timeLengthLayout=new QHBoxLayout();
    timeLengthLayout->addWidget(timelengthlb,0,Qt::AlignCenter);
    firstWid->setLayout(timeLengthLayout);

    threeButtonLayout = new QHBoxLayout();
//    threeButtonLayout->addWidget(itemPlay_PauseButton);//2020.11.12 隐藏此播放，有问题。。。
//    threeButtonLayout->addWidget(clipButton);//2020.11.12 隐藏此剪裁，后续开发。。。
    threeButtonLayout->addWidget(deleteButton);
    threeButtonWid->setLayout(threeButtonLayout);



    stackLayout=new QStackedLayout();//堆叠布局
    stackLayout->addWidget(firstWid);
    stackLayout->addWidget(threeButtonWid);
    stackWid->setLayout(stackLayout);

    splitLinestackWid->addWidget(line);
    splitLinestackWid->addWidget(playSlider);

    itemBottomLayout=new QHBoxLayout();
    itemBottomLayout->addWidget(fileName_dateTimeWid);
    itemBottomLayout->addWidget(stackWid,0,Qt::AlignRight);
    itemBottomLayout->setSpacing(0);
    itemBottomLayout->setMargin(0);
    itemBottomWid->setLayout(itemBottomLayout);

//    recordFileNamelb->setStyleSheet("font-size:14px;");
//    dateTimelb->setStyleSheet("font-size:14px;");



    itemLayout=new QVBoxLayout();//列表项布局
    itemLayout->addWidget(splitLinestackWid);
    itemLayout->addWidget(itemBottomWid);
    itemLayout->setMargin(0);
    itemLayout->setSpacing(0);
    itemsWid->setLayout(itemLayout);


//    itemsWid->setStyleSheet("background-color:pink;");
   // itemBottomWid->setStyleSheet("background-color:pink;");

    clipperstackWid->addWidget(itemsWid);
    clipperstackWid->addWidget(clipperWid);
    setAttribute(Qt::WA_TranslucentBackground);
    if(darkData->get("style-name").toString()=="ukui-dark"||darkData->get("style-name").toString()=="ukui-black")
    {
        this->setStyleSheet("background-color:#222222;"
                                "border-top-right-radius:0px;"
                                "border-bottom-right-radius:0px;");//后期适配主题颜s;
    }
    else
    {
//        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setStyleSheet("border-top-right-radius:0px;"
                            "border-bottom-right-radius:0px;");//自定义窗体(圆角+背景色)
    }


    connect(itemPlay_PauseButton,&QToolButton::clicked,this,&ItemsWindow::itemPlay_PauseClicked);
    connect(clipButton,&QToolButton::clicked,this,&ItemsWindow::clipper);
    connect(deleteButton,&QToolButton::clicked,this,&ItemsWindow::delFile);

    //setStyleSheet("border-radius:0px;");
    //fileName_dateTimeWid->setStyleSheet("background-color:blue;");

}
void ItemsWindow::clipperFun()
{
    //剪裁布局start

//    line=new QFrame(this);
//    line->setFrameShape(QFrame::HLine);
//    line->setStyleSheet("border-radius: 3px; background-color:red;");
    cutWaveWid=new QWidget(this);

    timelengthlb2=new QLabel(this);
    timelengthlb2->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}");


    cancelButton=new QToolButton(this);
    cancelButton->setText("取消");
    cancelButton->setFixedSize(40,25);
    //margin-left:0px;必须加上否则字体变白
    cancelButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                "QToolButton:hover{color:#F74175;}"
                                "QToolButton:pressed{color:red;}");
    finishButton=new QToolButton(this);
    finishButton->setText("完成");
    finishButton->setFixedSize(40,25);
    //margin-left:0px;必须加上否则字体变白
    finishButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                "QToolButton:hover{color:#F74175;}"
                                "QToolButton:pressed{color:red;}");

    connect(cancelButton,&QToolButton::clicked,this,&ItemsWindow::cancel);
    connect(finishButton,&QToolButton::clicked,this,&ItemsWindow::finish);



    clipperWid = new QWidget();//剪裁的总Wid
    bottomWid = new QWidget();//底部Wid

    waveLayout=new QHBoxLayout();
    //amplitudeNum.clear();
    for (int i=0;i<cutRectangleCount;i++)//频率直方图
    {
        myWave *wave=new myWave(this);//每次都要初始化一个矩形框
//      wave->setMaximumWidth(100);
        wave->setRange(0,100);
        mywave.push_back(wave);
        waveLayout->addWidget(wave);
        waveLayout->setMargin(0);
        waveLayout->setSpacing(1);

    }

    cutWaveWid->setLayout(waveLayout);
    cutWaveWid->setFixedHeight(20);



    bottomLayout = new QHBoxLayout();//底部布局
    bottomLayout->addWidget(timelengthlb2);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(finishButton);
    bottomWid->setLayout(bottomLayout);
    clipperLayout = new QVBoxLayout();//剪裁布局
    clipperLayout->addWidget(cutWaveWid,0,Qt::AlignCenter);
    clipperLayout->addWidget(bottomWid);
    clipperLayout->setSpacing(0);
    clipperLayout->setMargin(0);
    clipperWid->setLayout(clipperLayout);
    //剪裁布局end
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
                //splitLinestackWid->setCurrentIndex(nIndex%2);//2020.11.12隐藏此滑动条功能
            }
        }
        else if(mouseEvent->button() == Qt::RightButton)
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                qDebug()<<"按压了右键!";
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
    stopReplayer();
    player->play();
    itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/zanting.svg);border-radius: 16px; }"
                          "QToolButton:hover{image: url(:/svg/svg/zanting_select.svg);}"
                          "QToolButton:pressed{background-color:#F96E92;}");

    play_pause = true;
}
void ItemsWindow::playState()//播放状态
{
    if(player->state()!=QMediaPlayer::PlayingState)
    {
        return ;
    }
    player->pause();
    itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);border-radius: 16px; }"
                          "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}"
                          "QToolButton:pressed{background-color:#F96E92;}");
    play_pause=false;
}

void ItemsWindow::stopReplayer()//先暂停再播放！
{
    QList<ItemsWindow*> items=this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
    qDebug()<<this->parent()->findChildren<ItemsWindow*>();
    for(ItemsWindow *item:items)
    {
        item->playState();//判断播放状态

    }
}
//开始播放和暂停播放
void ItemsWindow::itemPlay_PauseClicked()
{
    MyThread *myth = new MyThread();
    QLabel *label = itemsWid->findChild<QLabel *>(dateTimelb->objectName());
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
                    player->setMedia(QUrl::fromLocalFile(str));
                    player->setVolume(50);
                    stopReplayer();//先暂停再播放
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
    QLabel *label = itemsWid->findChild<QLabel *>(dateTimelb->objectName());
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
    qDebug()<<"剪裁！" ;
    int nIndex = clipperstackWid->currentIndex();
    // 获取下一个需要显示的页面索引
    //qDebug()<<nIndex;
    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页s
    createCutWave();

    clipperstackWid->setCurrentIndex(nIndex%2);//切换至录音按钮stackLayout


}
void ItemsWindow::createCutWave()
{
    QStringList valueList;
    int array[110];
    for(int i=0;i<110;i++)
    {
        array[i]=0;
    }
    MyThread *myth = new MyThread();
    QLabel *label = itemsWid->findChild<QLabel *>(dateTimelb->objectName());
    //qDebug()<<label->text();
    //myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath=myth->readPathCollected().split(",");
    QString value=itemData->get("amplitude").toString();
    QStringList valueRowList = value.split(";");
    for(int i=0;i<valueRowList.length()-1;i++)
    {
        QString str = listRecordPath.at(i+1);
        //qDebug()<<valueRowList.at(i);//value从0开始
        if(str.contains(label->text()))
        {
            timelengthlb2->setText(MainWindow::mutual->playerTotalTime(listRecordPath.at(i+1)));
            //qDebug()<<"第:"<<i+1<<"个";
            QString str = valueRowList.at(i);
            valueList = str.split(",");
            //qDebug()<<valueList.length();
            for(int j=0;j<valueList.length()-1;j++)
            {
                QString str2=valueList.at(j);
                array[j]=str2.toInt();
            }
            amplitudeNum.clear();
            for(int i=0;i<cutRectangleCount;i++)
            {
                mywave.at(i)->setValue(array[i]/300);
            }
        }

    }

}
void ItemsWindow::cancel()
{
    qDebug()<<"取消";
    clipperstackWid->setCurrentIndex(0);//切换至录音按钮stackLayout
}
void ItemsWindow::finish()
{
    qDebug()<<"完成";
    clipperstackWid->setCurrentIndex(0);//切换至录音按钮stackLayout
}



