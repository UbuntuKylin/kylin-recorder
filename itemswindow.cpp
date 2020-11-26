#include "itemswindow.h"
#include "mainwindow.h"
ItemsWindow::ItemsWindow(QWidget *parent) : QMainWindow(parent)
{
    itemData = new QGSettings(KYLINRECORDER);
    darkData = new QGSettings(FITTHEMEWINDOW);

    initItemWid();//初始化ItemWid
    setItemWid();//设置ItemWid的界面
    initClipper();//初始化Clipper
    setClipper();//设置剪裁布局,点击裁剪时再显示裁剪波形图
    setAttribute(Qt::WA_TranslucentBackground);

}
void ItemsWindow::initItemWid()//初始化主界面
{

    playList = new QMediaPlaylist;//播放列表
    player = new QMediaPlayer;//媒体文件
    line = new QFrame();//分割线
    playSlider=new QSlider(this);//播放滑动条
    recordFileNamelb=new QLabel(this);//录音标号标签
    dateTimelb=new QLabel(this);//含有时间的文件名标签
    testlb=new QLabel(this);//占位
    timelengthlb=new QLabel(this);//时间长短的标签
    itemPlay_PauseButton=new QToolButton(this);//播放暂停按钮
    clipButton=new QToolButton(this);//剪裁按钮
    deleteButton=new QToolButton(this);//删除按钮
    playSlider->installEventFilter(this);//给playSlider安装事件过滤器


    itemsWid = new QWidget();//items主Wid
    itemsWid->installEventFilter(this);//给itemsWid安装事件过滤器
    itemTopWid = new QWidget();//item顶部Wid
    itemBottomWid = new QWidget();//item底部Wid
    fileName_dateTimeWid = new QWidget();//录音标号和文件名Wid
    threeButtonWid = new QWidget();//含有播放、剪裁和删除三个按钮的Wid
    firstWid=new QWidget();//时间长短第一层Wid
    stackWid=new QWidget();//堆叠Wid
    clipperstackWid=new QStackedWidget();//裁剪的堆叠Wid
    splitLinestackWid=new QStackedWidget();//分隔线Wid



    fileName_dateTimeLayout = new QVBoxLayout();//录音号和含有时间文件名布局
    timeLengthLayout=new QHBoxLayout();//时间长度的布局
    threeButtonLayout = new QHBoxLayout();//三个按钮布局
    stackLayout=new QStackedLayout();//堆叠布局
    itemBottomLayout=new QHBoxLayout();//item底部布局
    itemLayout=new QVBoxLayout();//列表项布局
   // itemsWid->setFixedHeight(60);

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

    connect(playSlider,&QSlider::valueChanged,this,&ItemsWindow::setPosition);
    connect(playSlider,SIGNAL(sliderPressed()),this,SLOT(slidePress()));
    connect(playSlider,SIGNAL(sliderReleased()),this,SLOT(slideRelease()));

    connect(itemPlay_PauseButton,&QToolButton::clicked,this,&ItemsWindow::itemPlay_PauseClicked);
    connect(clipButton,&QToolButton::clicked,this,&ItemsWindow::clipper);
    connect(deleteButton,&QToolButton::clicked,this,&ItemsWindow::delFile);

}
void ItemsWindow::setItemWid()//设置ItemWid的界面
{
    playSlider->setOrientation(Qt::Horizontal);
    playSlider->setFixedHeight(3);//后期要改成3
    playSlider->setStyleSheet("QSlider::handle:horizontal{width:2px;background-color:#F94A76;margin:-5px 0px -5px 0px;border-radius:1px;}"
                              "QSlider::groove:horizontal{height:3px;background-color:#EDEDED;}"
                              "QSlider::add-page:horizontal{background-color:#EDEDED;}"
                              "QSlider::sub-page:horizontal{background-color:#F94A76;}");

    recordFileNamelb->setStyleSheet("font-size:14px;");
    dateTimelb->setObjectName("dateTimelb");//设置对象名
    dateTimelb->setStyleSheet("font-size:14px;");
    testlb->setText(" ");
    timelengthlb->setText("00:00:00");
    itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);}"
                          "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}");
    itemPlay_PauseButton->setFixedSize(32,32);
    clipButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bianji.svg);}"
                              "QToolButton:hover{image: url(:/svg/svg/jianji_select.svg);}");
    clipButton->setFixedSize(32,32);
    deleteButton->setStyleSheet("QToolButton{image: url(:/svg/svg/delete.svg);}"
                                "QToolButton:hover{image: url(:/svg/svg/delete_select.svg);}");
    deleteButton->setFixedSize(32,32);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border-radius: 6px; background-color:#EFE4E1;");

    fileName_dateTimeLayout->addWidget(recordFileNamelb);
    fileName_dateTimeLayout->addWidget(dateTimelb);
    fileName_dateTimeLayout->setSpacing(0);
    fileName_dateTimeWid->setLayout(fileName_dateTimeLayout);

    timeLengthLayout->addWidget(timelengthlb,0,Qt::AlignCenter);
    firstWid->setLayout(timeLengthLayout);

    threeButtonLayout->addWidget(itemPlay_PauseButton);//2020.11.12 隐藏此播放，有问题。。。
//    threeButtonLayout->addWidget(clipButton);//2020.11.12 隐藏此剪裁，后续开发。。。
    threeButtonLayout->addWidget(deleteButton);
    threeButtonWid->setLayout(threeButtonLayout);

    stackLayout->addWidget(firstWid);
    stackLayout->addWidget(threeButtonWid);
    stackWid->setLayout(stackLayout);

    splitLinestackWid->addWidget(line);
    splitLinestackWid->addWidget(playSlider);


    itemBottomLayout->addWidget(fileName_dateTimeWid);
    itemBottomLayout->addWidget(stackWid,0,Qt::AlignRight);
    itemBottomLayout->setSpacing(0);
    itemBottomLayout->setMargin(0);
    itemBottomWid->setLayout(itemBottomLayout);

    itemLayout->addWidget(splitLinestackWid);
    itemLayout->addWidget(itemBottomWid);
    itemLayout->setMargin(0);
    itemLayout->setSpacing(0);
    itemsWid->setLayout(itemLayout);
    //itemsWid->setStyleSheet("bckground-color:pink;");
    qDebug()<<darkData->get("style-name").toString();
    qDebug()<<MainWindow::mutual->theme;
}
void ItemsWindow::initClipper()//初始化剪裁
{
    cutWaveWid=new QWidget(this);
    timelengthlb2=new QLabel(this);
    cancelButton=new QToolButton(this);
    finishButton=new QToolButton(this);

    clipperWid = new QWidget();//剪裁的总Wid
    bottomWid = new QWidget();//底部Wid

    waveLayout=new QHBoxLayout();//剪裁波形布局
    bottomLayout = new QHBoxLayout();//底部布局
    clipperLayout = new QVBoxLayout();//剪裁布局
    connect(cancelButton,&QToolButton::clicked,this,&ItemsWindow::cancel);
    connect(finishButton,&QToolButton::clicked,this,&ItemsWindow::finish);
}
void ItemsWindow::setClipper()//设置剪裁界面
{
    timelengthlb2->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}");
    cancelButton->setText(tr("Cancel"));
    cancelButton->setFixedSize(40,25);
    //margin-left:0px;必须加上否则字体变白
    cancelButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                "QToolButton:hover{color:#F74175;}"
                                "QToolButton:pressed{color:red;}");

    finishButton->setText(tr("Finish"));
    finishButton->setFixedSize(40,25);
    //margin-left:0px;必须加上否则字体变白
    finishButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                "QToolButton:hover{color:#F74175;}"
                                "QToolButton:pressed{color:red;}");
    //原来有个剪裁布局:改为点击剪裁按钮才显示



    bottomLayout->addWidget(timelengthlb2);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(finishButton);
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomWid->setLayout(bottomLayout);

    clipperLayout->addWidget(cutWaveWid,0,Qt::AlignCenter);
    clipperLayout->addWidget(bottomWid);
    clipperLayout->setSpacing(0);
    clipperLayout->setMargin(0);
    clipperWid->setLayout(clipperLayout);
    clipperstackWid->addWidget(itemsWid);

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
            stackLayout->setCurrentIndex(0);//3个按钮布局切换至显示时间label
            splitLinestackWid->setCurrentIndex(0);//播放条布局切换
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
              splitLinestackWid->setCurrentIndex(nIndex%2);//2020.11.12隐藏此滑动条功能
            }
        }
        else if(mouseEvent->button() == Qt::RightButton)
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                //rightClickedMenuRequest();11.26注释
            }

        }

    }
    return QObject::eventFilter(obj,event);
}

void ItemsWindow::rightClickedMenuRequest()//右击弹出Menu菜单
{
    qDebug()<<"按压了右键!弹出Menu菜单";
    menu = new QMenu();
    actionSave = new QAction();
    actionOpenFolder = new QAction();

    connect(actionSave,&QAction::triggered,this,&ItemsWindow::actionSaveasSlot);
    connect(actionOpenFolder,&QAction::triggered,this,&ItemsWindow::actionOpenFolderSlot);
    actionSave->setText(tr("Save as"));
    actionOpenFolder->setText(tr("Open folder position"));


    menu->addAction(actionSave);
    menu->addAction(actionOpenFolder);

    menu->exec(QCursor::pos());



    delete menu;
    delete actionSave;
    delete actionOpenFolder;
}

void ItemsWindow::actionSaveasSlot()
{
    qDebug()<<"另存为!";
}

void ItemsWindow::actionOpenFolderSlot()
{
    qDebug()<<"打开文件路径!";
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
    if(player->state()==QMediaPlayer::PlayingState)
    {
        qDebug()<<"有正在播放的音频";
        player->stop();
        itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bofang.svg);border-radius: 16px; }"
                              "QToolButton:hover{image: url(:/svg/svg/bofang_select.svg);}"
                              "QToolButton:pressed{background-color:#F96E92;}");
        play_pause=false;
    }


}

void ItemsWindow::stopReplayer()//先暂停再播放！
{
    //qDebug()<<"22222222222222222222222222222222222";
    QList<ItemsWindow*> items=this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
   // qDebug()<<this->parent()->findChildren<ItemsWindow*>();
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
                    WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                             ,tr("The file path does not exist or has been deleted!"),QMessageBox::Yes );
                    WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
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
                    WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                             ,tr("The file path does not exist or has been deleted!"),QMessageBox::Yes );
                    WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
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
    QStringList listRecordPath = myth->readPathCollected().split(",");
    QStringList listAmplitude = itemData->get("amplitude").toString().split(";");
    int m=myth->readNumList()-1;//因为配置文件初始为1
    if(m<=0)
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                 ,tr("The current number of list files is 0."),QMessageBox::Yes );
        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
        WrrMsg->exec();
        return ;
    }
    for(int i=1;i<=m;i++)
    {
        QString str=listRecordPath.at(i);
        //QString strTemp = listAmplitude.at(i-1);
        if(str.contains(label->text()))
        {
            QFileInfo fi(str);
            if(fi.exists())
            {
                QFileInfo fileinfo(str);
                QString filesuffix = fileinfo.suffix();//判断文件后缀
                if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
                {
                    qDebug()<<"文件存在!可以删除";
                    if(player->state()==QMediaPlayer::PlayingState)
                    {
                        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                                 ,tr("Playing, please stop and delete!"),QMessageBox::Yes );
                        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                        WrrMsg->exec();
                        return ;
                    }
                    QString subStr = ","+str;//子串
                    QString subAmplitudeStr = listAmplitude.at(i-1);
                    /*
                     * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串内容
                     */
                    QString oldStr = myth->recordData->get("recorderpath").toString();
                    int pos = oldStr.indexOf(subStr);
                    QString oldAmplitudeStr = itemData->get("amplitude").toString();
                    int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);

                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    myth->writePathCollected(newStr);
                    QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
                    itemData->set("amplitude",newAmplitudeStr);

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
                WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                         ,tr("The file path does not exist or has been deleted!"),QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
                return ;
            }

        }
    }
}
void ItemsWindow::clipper()
{
    qDebug()<<"剪裁！" ;
    clipperstackWid->addWidget(clipperWid);
    initRectangleWave();//就初始化1次就可以了
    createCutWave();
//    QStringList listRecordPath = itemData->get("recorderpath").toString().split(",");
//    QStringList listAmplitude = itemData->get("amplitude").toString().split(";");
//    qDebug()<<listAmplitude.length()<<"="<<listRecordPath.length();
    int nIndex = clipperstackWid->currentIndex();
    // 获取下一个需要显示的页面索引

    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页s

    clipperstackWid->setCurrentIndex(nIndex%2);//切换至录音按钮stackLayout
    qDebug()<<nIndex;

}
void ItemsWindow::createCutWave()
{

    QStringList valueList;
    int array[110];
    for(int i = 0;i<110;i++)
    {
        array[i] = 0;
    }
    MyThread *myth = new MyThread();
    QLabel *label = itemsWid->findChild<QLabel *>(dateTimelb->objectName());
    //qDebug()<<label->text();
    //myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath = myth->readPathCollected().split(",");
    QString value = itemData->get("amplitude").toString();
    QStringList valueRowList = value.split(";");
    for(int i = 0;i<valueRowList.length()-1;i++)
    {
        QString str = listRecordPath.at(i+1);
        qDebug()<<valueRowList.at(i);//value从0开始
        if(str.contains(label->text()))
        {
            timelengthlb2->setText(MainWindow::mutual->playerTotalTime(listRecordPath.at(i+1)));
            //qDebug()<<"第:"<<i+1<<"个";
            QString str = valueRowList.at(i);
            valueList = str.split(",");
            //qDebug()<<valueList.length();
            for(int j = 0;j<valueList.length()-1;j++)
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
    stackLayout->setCurrentIndex(0);//3个按钮布局切换至显示时间label
    splitLinestackWid->setCurrentIndex(0);//播放条布局切换
    mywave.clear();
    //deleteWaves();
    //initRectangleWave();//就初始化1次就可以了
}
void ItemsWindow::finish()
{
    qDebug()<<"完成";

    clipperstackWid->setCurrentIndex(0);//切换至录音按钮stackLayout
    stackLayout->setCurrentIndex(0);//3个按钮布局切换切换至显示时间label
    splitLinestackWid->setCurrentIndex(0);//播放条布局切换
    //deleteWaves();
}
void ItemsWindow::initRectangleWave()//初始化矩形框
{   
    if(initTag==0)
    {
        initTag=1;
        for (int i=0;i<cutRectangleCount;i++)//频率直方图
        {
            myWave *wave=new myWave(this);//每次都要初始化一个矩形框
            wave->setRange(0,100);
            mywave.append(wave);
            waveLayout->addWidget(wave);
        }

    }
    waveLayout->setMargin(0);
    waveLayout->setSpacing(1);
    cutWaveWid->setLayout(waveLayout);
    cutWaveWid->setFixedHeight(50);

}
void ItemsWindow::deleteWaves()
{
    for(myWave *wave :mywave)
    {
        wave->deleteLater();
    }
}


