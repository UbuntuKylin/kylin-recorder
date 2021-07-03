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
#include "itemswindow.h"
#include "mainwindow.h"
#include "clipbutton.h"

#include "tools.h"

ItemsWindow::ItemsWindow(QWidget *parent) : QMainWindow(parent)
{
    itemData = new QGSettings(KYLINRECORDER);
    //    darkData = new QGSettings(FITTHEMEWINDOW);
    initItemWid();//初始化ItemWid
    setItemWid();//设置ItemWid的界面
    initClipper();//初始化Clipper
    setClipper();//设置剪裁布局,点击裁剪时再显示裁剪波形图
    initThemeGsetting();//初始化主题的配置文件
    setAttribute(Qt::WA_TranslucentBackground);

}

void ItemsWindow::initItemWid()//初始化主界面
{
    line = new QFrame();//分割线
    processDEL = new QProcess;
    playSlider = new QSlider(this);//播放滑动条
    playSlider->installEventFilter(this);//给playSlider安装事件过滤器
    Record_Time = new QLabel(this);//录音标号标签
    RecordTimeChangeColor = new QLabel(this);//颜色
    recordFileName = new QLabel(this);//含有时间的文件名标签
    recordFileNameChangeColor = new QLabel(this);//颜色
    testlb = new QLabel(this);//占位
    timelengthlb = new QLabel(this);//时间长短的标签
    itemPlay_PauseButton = new QToolButton(this);//播放暂停按钮
    clipButton = new QToolButton(this);//剪裁按钮
    deleteButton = new QToolButton(this);//删除按钮
    //deleteButton->setFocusPolicy(Qt::ClickFocus);

    itemsWid = new QWidget(this);//items主Wid
    itemsWid->setFocusPolicy(Qt::ClickFocus);
    itemsWid->installEventFilter(this);//给itemsWid安装事件过滤器

    itemTopWid = new QWidget();//item顶部Wid
    itemBottomWid = new QWidget();//item底部Wid
    fileName_dateTimeWid = new QWidget();//录音标号和文件名Wid
    threeButtonWid = new QWidget();//含有播放、剪裁和删除三个按钮的Wid
    firstWid = new QWidget();//时间长短第一层Wid
    stackWid = new QWidget();//堆叠Wid
    clipperstackWid = new QStackedWidget();//裁剪的堆叠Wid
    splitLinestackWid = new QStackedWidget();//分隔线Wid

    fileName_dateTimeLayout = new QVBoxLayout();//录音号和含有时间文件名布局
    timeLengthLayout = new QHBoxLayout();//时间长度的布局
    threeButtonLayout = new QHBoxLayout();//三个按钮布局
    stackLayout = new QStackedLayout();//堆叠布局
    itemBottomLayout = new QHBoxLayout();//item底部布局
    itemLayout = new QVBoxLayout();//列表项布局

    connect(MainWindow::mutual->mpvPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(MainWindow::mutual->mpvPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));
    connect(MainWindow::mutual->mpvPlayer,SIGNAL(stateChanged(MMediaPlayer::State)), this, SLOT(stateChanged(MMediaPlayer::State)));

    connect(playSlider,&QSlider::valueChanged,this,&ItemsWindow::setPosition);
    connect(playSlider,SIGNAL(sliderPressed()),this,SLOT(slidePress()));
    connect(playSlider,SIGNAL(sliderReleased()),this,SLOT(slideRelease()));

    connect(itemPlay_PauseButton,&QToolButton::clicked,this,&ItemsWindow::itemPlay_PauseClicked);
    connect(clipButton,&QToolButton::clicked,this,&ItemsWindow::clipper);
    connect(deleteButton,&QToolButton::clicked,this,&ItemsWindow::delFile);

    connect(this,&ItemsWindow::playingSignal,MainWindow::mutual,&MainWindow::getPlayingSlot);


}

void ItemsWindow::setItemWid()//设置ItemWid的界面
{
    playSlider->setOrientation(Qt::Horizontal);
    playSlider->setFixedHeight(12);//可以让鼠标点击滑动条进度的区域变大容错率高
    //height:3px;滑条的样式宽度根据设计稿
    playSlider->setStyleSheet("QSlider::groove:horizontal{height:3px;background-color:#EDEDED;}"
                              "QSlider::add-page:horizontal{background-color:#EDEDED;}"
                              "QSlider::sub-page:horizontal{background-color:#3790FA;}");

    Record_Time->setObjectName("Record_Time");
    Record_Time->setStyleSheet("font-size:14px;");//录音序号

    recordFileName->setObjectName("dateTimelb");//设置对象名
    recordFileName->setStyleSheet("font-size:14px;");//录音文件名
    testlb->setText(" ");
    timelengthlb->setText("00:00:00");
    timelengthlb->setStyleSheet("font-size : 14px");
    itemPlay_PauseButton->setFixedSize(32,32);
    itemPlay_PauseButton->setToolTip(tr("play"));
//    itemPlay_PauseButton->setToolTip(tr("play/pause"));

    clipButton->setFixedSize(32,32);
    clipButton ->setToolTip(tr("clip"));

    deleteButton ->setFixedSize(32,32);
    deleteButton ->setToolTip(tr("delete"));
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color:#EDEDED;");

    fileName_dateTimeLayout->addWidget(recordFileName);
    fileName_dateTimeLayout->addWidget(Record_Time);
    fileName_dateTimeLayout->setSpacing(0);
    fileName_dateTimeLayout->setContentsMargins(0,0,0,0);//调整这俩的外间距
    fileName_dateTimeWid->setLayout(fileName_dateTimeLayout);

    timeLengthLayout->addWidget(timelengthlb,0,Qt::AlignBottom);
    firstWid->setLayout(timeLengthLayout);

    threeButtonLayout->addWidget(itemPlay_PauseButton);//2020.11.20 由于依赖问题此播放问题解决。
//    threeButtonLayout->addWidget(clipButton);//2020.12.21 剪裁暂时禁用,为稳定版本，后续优化界面。。。
    threeButtonLayout->addWidget(deleteButton);
    threeButtonWid->setLayout(threeButtonLayout);

    stackLayout->addWidget(firstWid);
    stackLayout->addWidget(threeButtonWid);
    stackWid->setLayout(stackLayout);

    splitLinestackWid->addWidget(line);
    splitLinestackWid->addWidget(playSlider);

    itemBottomLayout->addWidget(fileName_dateTimeWid);//录音名与录制时间的布局
    itemBottomLayout->addWidget(stackWid,0,Qt::AlignRight);//按钮和时间长度的堆叠布局
    itemBottomLayout->setSpacing(0);
    itemBottomLayout->setContentsMargins(30,0,20,0);
    itemBottomWid->setLayout(itemBottomLayout);

    itemLayout->addWidget(splitLinestackWid);
    itemLayout->addWidget(itemBottomWid);
    itemLayout->setMargin(0);
    itemLayout->setSpacing(0);
    itemsWid->setLayout(itemLayout);

}

void ItemsWindow::initClipper()//初始化剪裁
{
    railWid = new QWidget(this);
    cutWaveWid = new QWidget(this);

    timelengthlb2 = new QLabel(this);
    cancelButton = new QToolButton(this);
    finishButton = new QToolButton(this);
    leftBtn = new ClipButton(this);
    rightBtn = new ClipButton(this);
    cursor = new ClipButton(this);//游标
    moveTime = new QTimer;

    clipperWid = new QWidget();//剪裁的总Wid
    bottomWid = new QWidget();//底部Wid

    rectangleLayout = new QHBoxLayout();
    waveLayout = new QHBoxLayout();//剪裁波形布局
    bottomLayout = new QHBoxLayout();//底部布局
    clipperLayout = new QVBoxLayout();//剪裁布局

    connect(cancelButton,&QToolButton::clicked,this,&ItemsWindow::cancel);
    connect(finishButton,&QToolButton::clicked,this,&ItemsWindow::finish);

    connect(leftBtn,&ClipButton::leftButton_rightBorderSignal,this,&ItemsWindow::leftButton_rightBorderSlot);
    connect(leftBtn,&ClipButton::pressPausePlayer_Signal,this,&ItemsWindow::pressPausePlayer_Slot);
    connect(leftBtn,&ClipButton::leftBtn_ReleaseStartPlayer_Signal,this,&ItemsWindow::leftBtn_ReleaseStartPlayer_Slot);
    connect(rightBtn,&ClipButton::rightBtn_ReleaseGetEndPositon_Signal,this,&ItemsWindow::rightBtn_ReleaseGetEndPositon_Slot);
    connect(moveTime,&QTimer::timeout,this,&ItemsWindow::cursorMove);

}

void ItemsWindow::setClipper()//设置剪裁界面
{
    timelengthlb2->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}");
    cancelButton->setText(tr("Cancel"));
    cancelButton->setFixedSize(40,25);

    finishButton->setText(tr("Finish"));
    finishButton->setFixedSize(40,25);

    //原来有个剪裁布局:改为点击剪裁按钮才显示
    bottomLayout->addWidget(timelengthlb2);
//    bottomLayout->addWidget(liveTime);
    bottomLayout->addWidget(cancelButton);
    bottomLayout->addWidget(finishButton);
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomWid->setLayout(bottomLayout);

    clipperLayout->addWidget(cutWaveWid);
    clipperLayout->addWidget(bottomWid);
    clipperLayout->setSpacing(0);
    clipperLayout->setMargin(0);
    clipperWid->setLayout(clipperLayout);

    clipperstackWid -> addWidget(itemsWid);
//    clipperstackWid -> addWidget(clipperWid);//0619暂时注释

}
ItemsWindow::~ItemsWindow()
{
    delete wave;
}

void ItemsWindow::initThemeGsetting()
{
    connect(MainWindow::mutual->themeData, &QGSettings::changed, this, [=]  ()
    {
        itemsThemeColor = MainWindow::mutual->themeData->get("style-name").toString();
        themeStyle(itemsThemeColor);
    });
    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());
}

void ItemsWindow::themeStyle(QString themeColor)
{
    if(themeColor == "ukui-dark"||themeColor=="ukui-black")
    {
        Record_Time->setStyleSheet("font-size:14px;color:white;");
        recordFileName->setStyleSheet("font-size:14px;color:white;");
        clipButton->setStyleSheet("QToolButton{image: url(:/svg/svg/dark_clip.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/jianji_select.svg);}");
        deleteButton->setStyleSheet("QToolButton{image: url(:/svg/svg/dark_delete.svg);}"
                                    "QToolButton:hover{image: url(:/svg/svg/delete_hover.svg);}"
                                    "QToolButton:pressed{image: url(:/svg/svg/delete_click.svg);}");
        //margin-left:0px;必须加上否则字体变白
        cancelButton->setStyleSheet("QToolButton{margin-left:0px;color:#FFFFFF;}"
                                    "QToolButton:hover{color:#F74175;}"
                                    "QToolButton:pressed{color:red;}");
        //margin-left:0px;必须加上否则字体变白
        finishButton->setStyleSheet("QToolButton{margin-left:0px;color:#FFFFFF;}"
                                    "QToolButton:hover{color:#F74175;}"
                                    "QToolButton:pressed{color:red;}");
        line->setStyleSheet("background-color:#222222;");
        if(play_pause)
        {
            itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-dark.svg);}"
                                                "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                                                "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        }
        else
        {
            itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continu-dark.svg);}"
                                                "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                                "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
        }

    }
    else
    {
        Record_Time->setStyleSheet("font-size:14px;color:black;");
        recordFileName->setStyleSheet("font-size:14px;color:black;");
        clipButton->setStyleSheet("QToolButton{image: url(:/svg/svg/bianji.svg);}"
                                  "QToolButton:hover{image: url(:/svg/svg/jianji_select.svg);}");
        deleteButton->setStyleSheet("QToolButton{image: url(:/svg/svg/delete-defalut-light.svg);}"
                                    "QToolButton:hover{image: url(:/svg/svg/delete_hover.svg);}"
                                    "QToolButton:pressed{image: url(:/svg/svg/delete_click.svg);}");

        //margin-left:0px;必须加上否则字体变白
        cancelButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                    "QToolButton:hover{color:#F74175;}"
                                    "QToolButton:pressed{color:red;}");
        line->setStyleSheet("background-color:#EDEDED;");
        //margin-left:0px;必须加上否则字体变白
        finishButton->setStyleSheet("QToolButton{margin-left:0px;color:#303133;}"
                                    "QToolButton:hover{color:#F74175;}"
                                    "QToolButton:pressed{color:red;}");
        if(play_pause)
        {
            itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/pause-light.svg);}"
                                                "QToolButton:hover{image: url(:/svg/svg/pause-hover.svg);}"
                                                "QToolButton:pressed{image: url(:/svg/svg/pause-click.svg);}");
        }
        else
        {
            itemPlay_PauseButton->setStyleSheet("QToolButton{image: url(:/svg/svg/continue-light.svg);}"
                                                "QToolButton:hover{image: url(:/svg/svg/continue-hover.svg);}"
                                                "QToolButton:pressed{image: url(:/svg/svg/continue-click.svg);}");
        }
    }
}

void ItemsWindow::positionChange(qint64 position)
{
    if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PlayingState)
    {

        QTime currentTime(static_cast<int64_t>(position) / (60*60*1000) ,
                          static_cast<int64_t>(position) % (60*60*1000) / 60000,
                          static_cast<int64_t>((position % (60*1000)) / 1000.0));
        QString current_timeStr = currentTime.toString("hh:mm:ss");
        if(static_cast<int64_t>(MainWindow::mutual->mpvPlayer->duration()<4000000))
        {
            QTime totalTime(static_cast<int64_t>(MainWindow::mutual->mpvPlayer->duration()) / (60*60*1000),
                            static_cast<int64_t>(MainWindow::mutual->mpvPlayer->duration())% (60*60*1000) / 60000,
                            static_cast<int64_t>(MainWindow::mutual->mpvPlayer->duration()% (60*1000) / 1000.0));

//            qDebug()<<current_timeStr + "/" + totalTime.toString("hh:mm:ss");//输出播放进度
        }
        else
        {
            QTime totalTime(static_cast<int64_t>(durTime*890) / (60*60*1000),
                            static_cast<int64_t>(durTime*890)% (60*60*1000) / 60000,
                            static_cast<int64_t>(durTime*890 % (60*1000) / 1000.0));
//            qDebug()<<current_timeStr + "/" + totalTime.toString("hh:mm:ss");//输出播放进度
        }

        playSlider->setValue(static_cast<int64_t>(position));

    }

}

void ItemsWindow::durationChange(qint64 duration)   //更新播放进度
{
//    qDebug()<<"duration:"<<static_cast<int>(duration);
    if(static_cast<int>(duration)<4000000)
    {
        playSlider->setRange(0,static_cast<int>(duration));
        playSlider->setEnabled(duration>0);
        playSlider->setPageStep(static_cast<int>(duration)/10);
    }
    else
    {
        playSlider->setRange(0,durTime*890);
        playSlider->setEnabled(durTime*890>0);
        playSlider->setPageStep(static_cast<int>(durTime*890)/10);
    }

}

void ItemsWindow::stateChanged(enum MMediaPlayer::State)
{

    QList<ItemsWindow*> items = this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
    //qDebug()<<this->parent()->findChildren<ItemsWindow*>();
    for(ItemsWindow *item:items)
    {
        if(filePath == MainWindow::mutual->tempPath)
        {
            if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::StoppedState)
            {
//                qDebug()<<"列表播放结束 停止" <<MainWindow::mutual->tempPath;
                item->itemPlay_PauseButton->setToolTip(tr("play"));
                item->play_pause=false;
                item->themeStyle(MainWindow::mutual->themeData->get("style-name").toString());
                emit playingSignal(false);//播放结束才可以点击录音
                item->splitLinestackWid->setCurrentIndex(0);//切换至分割线
            }
            else if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PlayingState)
            {
//                qDebug()<<"播放....." <<MainWindow::mutual->tempPath;
                itemPlay_PauseButton->setToolTip(tr("pause"));
                play_pause=true;
                themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
                splitLinestackWid->setCurrentIndex(1);//切换至进度条
                emit playingSignal(true);//播放结束才可以点击录音
                return ;
            }

        }
    }
}

void ItemsWindow::setPosition(int position)
{
    if (qAbs(MainWindow::mutual->mpvPlayer->position() - position) > 99)
        MainWindow::mutual->mpvPlayer->setPosition(position);
//    qDebug()<<"数值:"<<position;
}
bool ItemsWindow::eventFilter(QObject *obj, QEvent *event)   //鼠标滑块点击
{

    if(obj == playSlider)
    {
        if (event->type() == QEvent::MouseButtonPress)           //判断类型
        {
            qDebug()<<"左键";
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) //判断左键
            {
               qDebug()<<"maximum"<<playSlider->maximum()<<"minimum"<<playSlider->minimum()<<"mouseEvent->x()"<<mouseEvent->x()<<"playSlider->width()"<<playSlider->width()<<"playSlider->sliderPosition()"<<playSlider->sliderPosition();
               int dur = playSlider->maximum() - playSlider->minimum();
//               int pos = (double)playSlider->minimum() + (double)dur * ((double)mouseEvent->x() / (double)playSlider->width());
               int value = playSlider->minimum() + ((playSlider->maximum() - playSlider->minimum()) * ((double)mouseEvent->x())) / (playSlider->width());
               qDebug()<<"位置:"<<value;
               if(value != playSlider->sliderPosition())
               {
                   playSlider->setValue(value);

               }
               //极其重要：此行代码必须加，用于鼠标点击时选中播放的歌曲进度
               MainWindow::mutual->mpvPlayer->setMedia(QUrl::fromLocalFile(filePath));
            }
        }
    }
    else if(obj == itemsWid)//悬浮特效、点击特效、右击特效
    {
        if(!MainWindow::mutual->isRecording)
        {
            itemsWid->setAttribute(Qt::WA_Hover,true);//****关键代码,若不加此行代码则没有悬浮特效****
            hover_ChangeState(event);//悬浮item时的变化
        }
        else
        {
            itemsWid->setAttribute(Qt::WA_Hover,false);
        }
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent->button() == Qt::RightButton)
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                rightClickedMenuRequest();//右击弹窗:1另存为、2打开文件
            }
        }

    }
    return QWidget::eventFilter(obj,event);
}

void ItemsWindow::hover_ChangeState(QEvent *event)
{
    RecordTimeChangeColor = itemsWid->findChild<QLabel *>(Record_Time->objectName());
    recordFileNameChangeColor = itemsWid->findChild<QLabel *>(recordFileName->objectName());
    if(event->type() == QEvent::HoverEnter||event->type() == QEvent::HoverMove)//显示浮窗
    {
//        qDebug()<<"进入";
        RecordTimeChangeColor->setStyleSheet("font-size:14px;color:#3790FA;");
        recordFileNameChangeColor->setStyleSheet("font-size:14px;color:#3790FA;");
        int nIndex = stackLayout->currentIndex();
        nIndex++;
        stackLayout->setCurrentIndex(1);//切换至录音按钮stackLayout
    }
    else if(event->type() == QEvent::HoverLeave)//收起浮窗
    {
//        qDebug()<<"离开";
        //此处为鼠标悬停离开某item时,字体颜色变化要注意和主题对应黑主题白字白主题黑字
        if(MainWindow::mutual->themeData->get("style-name").toString() == "ukui-dark"||MainWindow::mutual->themeData->get("style-name").toString() == "ukui-black")
        {
            RecordTimeChangeColor->setStyleSheet("font-size:14px;color:white;");
            recordFileNameChangeColor->setStyleSheet("font-size:14px;color:white;");
        }
        else
        {
            RecordTimeChangeColor->setStyleSheet("font-size:14px;color:black;");
            recordFileNameChangeColor->setStyleSheet("font-size:14px;color:black;");
        }
        stackLayout->setCurrentIndex(0);//切换至录音按钮stackLayout
//        splitLinestackWid->setCurrentIndex(0);//切换至分割线
    }
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
    menu->addAction(actionOpenFolder);//2020/12/12禁用
    menu->exec(QCursor::pos());

    menu->deleteLater();
    actionSave->deleteLater();
    actionOpenFolder->deleteLater();
}

//右键另存为其他路径
void ItemsWindow::actionSaveasSlot()
{

    QString oldFilePath = this->filePath;
    MyThread *my = new MyThread;//要用MyThread类的系统弹出框，由于ItemsWindowd的弹出框样式变了
    if(Tools::fileExists(oldFilePath))
    {
        my->saveAs(oldFilePath);
    }
    else
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                             tr("The file path does not exist or has been deleted!"));
    }
    my->deleteLater();
}

//右键打开文件位置并选中
void ItemsWindow::actionOpenFolderSlot()
{
    QString openedPath = this->filePath;
//    MyThread *my = new MyThread;
//    if(Tools::fileExists(openedPath))
//    {
//        my->OpenFolderProcess(openedPath);
//    }
//    else
//    {
//        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
//                             tr("The file path does not exist or has been deleted!"));
//    }
//    my->deleteLater();

    QProcess process;
    QString str = "\"" + openedPath +"\"";
    process.startDetached("peony --show-items " + str);
}

void ItemsWindow::slidePress() //滑动条鼠标按下
{
    qDebug()<<"按下了我";
//    MainWindow::mutual->mpvPlayer->setMedia(QUrl::fromLocalFile(filePath));
}
void ItemsWindow::slideRelease()   //滑动条鼠标弹起
{
//    stopReplayer();
    qDebug()<<"弹起了我";
//    MainWindow::mutual->mpvPlayer->setMedia(QUrl::fromLocalFile(filePath));
//    MainWindow::mutual->mpvPlayer->play();
//    play_pause = true;
//    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
}
void ItemsWindow::playState()//播放状态
{

    if(MainWindow::mutual->mpvPlayer->state()==MMediaPlayer::PlayingState)
    {
        qDebug()<<"有正在播放的音频1";
        MainWindow::mutual->mpvPlayer->stop();
        play_pause=false;
        themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
    }

}

void ItemsWindow::isOtherClipWidOpen()
{
    if(clipperstackWid ->currentIndex()!=0)//若有其他的剪裁界面则先将其他的收回去
    {
        qDebug()<<"有其他剪裁界面的";
        moveTime->stop();//游标随时间移动也要停止
        MainWindow::mutual->mpvPlayer->stop();
        clipperstackWid ->setCurrentIndex(0);
        stackLayout->setCurrentIndex(0);//切换至录音按钮stackLayout
        splitLinestackWid->setCurrentIndex(0);//2020.11.12隐藏此滑动条功能
        play_pause=false;
        themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
    }
}

void ItemsWindow::stopReplayer()//先暂停再播放！
{
    QList<ItemsWindow*> items = this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
    //qDebug()<<this->parent()->findChildren<ItemsWindow*>();
    for(ItemsWindow *item:items)
    {
        item->playState();//判断播放状态
    }
}

void ItemsWindow::getRecordingSlot(bool recordingState)
{
//    isRecording =
}

void ItemsWindow::itemPlay_PauseClicked()//开始播放和暂停播放
{  
    int fileCount = Tools::getRecordingFileinfos().count();//获取不同目录下的所有文件个数
    int index = this->parent()->findChildren<ItemsWindow*>().indexOf(this);
    qDebug()<<"选择的是："<<this->filePath<<"items中第"<<index<<"个";
    qDebug()<<"文件总个数:"<<fileCount;
    QString isPlayedPath = this->filePath;

    if(Tools::fileExists(isPlayedPath))
    {
        if(MainWindow::mutual->tempPath != isPlayedPath)//不是原路径的音频文件时
        {
            qDebug()<<"不是原路径的音频文件时"<<MainWindow::mutual->tempPath<<" "<<isPlayedPath;
            MainWindow::mutual->tempPath = isPlayedPath;
            qDebug()<<"之前的状态:"<<MainWindow::mutual->mpvPlayer->state();
            MainWindow::mutual->mpvPlayer->stop();
            qDebug()<<"stop之后"<<MainWindow::mutual->mpvPlayer->state();
            judgeState(MainWindow::mutual->mpvPlayer->state(),isPlayedPath);
            return ;
        }
        else
        {
            qDebug()<<"是原路径的音频文件时";
//            MainWindow::mutual->playerCompoment->pause();
            judgeState(MainWindow::mutual->mpvPlayer->state(),isPlayedPath);
            return ;
        }
    }
    else
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                             tr("The file path does not exist or has been deleted!"));
    }
    emit playingSignal(false);

    return ;

}

//原路径音频文件状态
void ItemsWindow::judgeState(enum MMediaPlayer::State,QString path)
{
    /* FFUtil fu这一行-durTime这一行必须放在judgeState里,因为涉及到文件选择录音打开时
     * 会直接进这个函数进来后就要计算时间长度。而不是点击播放按钮再计算
     * */
    FFUtil fu;
    fu.open(path);
    QFile file(path);
    int t_duration = fu.getDuration(path);
    durTime=t_duration>30000?(file.size()/64000):t_duration;
    qDebug()<<"你点击的路径"<<path<<"时长"<<durTime;
    qDebug()<<"播放状态"<<MainWindow::mutual->mpvPlayer->state();

    if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PlayingState)
    {
        MainWindow::mutual->mpvPlayer->pause();
        itemPlay_PauseButton->setToolTip(tr("play"));
        splitLinestackWid->setCurrentIndex(0);//点击暂停时在显示那个分割线
        play_pause=false;
        themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
        emit playingSignal(false);//点击暂停之后才可以再点击录音
        qDebug()<<"原路径的音频文件时，暂停"<<path<<"状态"<<MainWindow::mutual->mpvPlayer->state();
    }
    else if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PausedState)
    {

        if(!isOpen)
        {
            isOpen = true;
            emit playingSignal(true);

        }
        MainWindow::mutual->mpvPlayer->play();
        itemPlay_PauseButton->setToolTip(tr("pause"));
        splitLinestackWid->setCurrentIndex(1);//点击播放时在显示那个进度条
        play_pause = true;
        themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
        qDebug()<<"原路径的音频文件时，开始"<<path<<"状态"<<MainWindow::mutual->mpvPlayer->state();
    }
    else
    {
        qDebug()<<"这是停止";
        qDebug()<<"要开始播放的路径"<<path<<"play前状态"<<MainWindow::mutual->mpvPlayer->state();
        //根据录音实际将以下2行放在此处有重要原因,setMedia用于stop转play时才可以用
        MainWindow::mutual->mpvPlayer->setMedia(QUrl::fromLocalFile(path));
        MainWindow::mutual->mpvPlayer->setVolume(85);
        stop = true;
        emit playingSignal(true);
        MainWindow::mutual->mpvPlayer->play();
        qDebug()<<"play之后状态"<<MainWindow::mutual->mpvPlayer->state();
        itemPlay_PauseButton->setToolTip(tr("pause"));
        splitLinestackWid->setCurrentIndex(1);//点击播放时在显示那个进度条
        play_pause = true;
        themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
    }
}

//删除音频文件至回收站
void ItemsWindow::delFile()
{
    int fileCount = Tools::getRecordingFileinfos().count();//获取不同目录下的所有文件个数
    QString delFilePath = this->filePath;//filePath作为itemswindow的一个属性
    qDebug()<<"删除的是："<<delFilePath;//前提是鼠标悬浮有焦点之后才可以锁定此itemswindow的属性
    qDebug()<<"文件个数："<<fileCount;
    if(Tools::fileExists(delFilePath))
    {
        if(MainWindow::mutual->mpvPlayer->state()==MMediaPlayer::PlayingState)
        {
            QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                                 tr("Playing, please stop and delete!"));
            return ;
        }
        qDebug()<<"输出删除之前的Item地址"<<this->parent()->findChildren<ItemsWindow*>();
        int x = this->parent()->findChildren<ItemsWindow*>().indexOf(this);
        this->deleteLater();//删除时,必须要加上因为一个item就是一个类
        MainWindow::mutual->list->takeItem(MainWindow::mutual->list->count()-1-x);//删除操作
        qDebug()<<"路径存在，删除"<<delFilePath;
        MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());//传item个数
        deleteImage(delFilePath);//移入回收站

        return ;

    }
    else
    {

        int y = this->parent()->findChildren<ItemsWindow*>().indexOf(this);
        this->deleteLater();//删除时,必须要加上因为一个item就是一个类
        qDebug()<<"第"<<y<<"个不存在,直接删除:";
        MainWindow::mutual->list->takeItem(MainWindow::mutual->list->count()-1-y);//删除list列表的item操作
        MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());//传item个数
        listFileNumUpdate(MainWindow::mutual->list->count());

        return ;
    }
}

//2021.06.21更新配置文件的接口,暂时不用
void ItemsWindow::delUpdateGSetting(QString fileName)
{
    //进这里是因为文件不存在，但配置文件的路径还未被更新
    qDebug()<<"不存在的文件路径是"<<fileName;
    qDebug()<<"此时num数:"<<itemData->get("num").toInt();
    QStringList listRecordPath = itemData->get("recorderpath").toString().split(",");
    qDebug()<<listRecordPath;
    if(listRecordPath.contains(fileName))
    {
        qDebug()<<"第"<<listRecordPath.indexOf(fileName)<<"个文件"<<fileName<<"被删除！";
        QString subStr = ","+fileName;//子串
        QString oldStr=itemData->get("recorderpath").toString();
        int pos=oldStr.indexOf(subStr);
        qDebug()<<"被删除的路径在老串中的位置:"<<pos;
        qDebug()<<"删除的子串"<<oldStr.mid(pos,fileName.length()+1);
        QString newStr = oldStr.remove(pos,fileName.length()+1);
        itemData->set("recorderpath",newStr);
        itemData->set("num",itemData->get("num").toInt()-1);
        qDebug()<<"删除后的配置文件中路径集:"<<itemData->get("recorderpath").toString();
        qDebug()<<"删除后的个数:"<<itemData->get("num").toInt();
    }
}

void ItemsWindow::deleteImage(QString savepath)
{
    _processStart("gio",QStringList() << "trash" << savepath);
}

void ItemsWindow::_processStart(QString cmd, QStringList arguments)
{
    processDEL->start(cmd,arguments);
    processDEL->waitForStarted();
    processDEL->waitForFinished();
}

void ItemsWindow::processLog()
{
    qDebug()<<"*******process error*******\n"
           << QString::fromLocal8Bit(processDEL->readAllStandardError())
           <<"\n*******process error*******";
}

//2021.06.21暂时不用
QString ItemsWindow::executeLinuxCmd(QString strCmd)
{
    QProcess p;
    p.start("bash", QStringList() <<"-c" << strCmd);
    p.waitForFinished();
    QString strResult = p.readAllStandardOutput();
    qDebug()<<strResult;
    return strResult;
}

//列表文件数更新
void ItemsWindow::listFileNumUpdate(int num)
{
    int i = 1;
    QList<ItemsWindow*> items = this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
    qDebug()<<"Item个数:"<<items.count();

}

void ItemsWindow::clipper()
{
    qDebug()<<"剪裁！";
    cursorCanMove = 1;//刚开始点剪裁时默认未点击取消或完成,游标可以移动
    int nIndex = clipperstackWid -> currentIndex();
    QList<ItemsWindow*> items = this->parent()->findChildren<ItemsWindow*>();//取此类的父类的所有ItemsWindow类；findChildren：找所有的子类，findChild为找一个子类
    qDebug()<<"item的个数："<<items.count();
    for(ItemsWindow *item:items)
    {
        item->isOtherClipWidOpen();
    }
    initRectangleWave();//就初始化1次就可以了
    int i = createCutWave();
    // 获取下一个需要显示的页面索引
    nIndex++;
    // 当需要显示的页面索引大于等于总页面时，切换至首页s
    if(i==0)//i=0说明配置文件中有指定路径下音频文件的波形图;i=-1说明音频文件已经被删除
    {
        clipperstackWid->setCurrentIndex(nIndex%2);//切换至录音按钮stackLayout
        qDebug()<<nIndex;
    }
}

int ItemsWindow::createCutWave()
{
    QStringList valueList;
    int array[ITEMWINDOW_RECTANGLE_COUNT];
    for(int i = 0;i<ITEMWINDOW_RECTANGLE_COUNT;i++)
    {
        array[i] = 0;
    }
    MyThread *myth = new MyThread();
    QLabel *label = itemsWid->findChild<QLabel *>(recordFileName->objectName());
    //qDebug()<<label->text();
    //myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath = myth->readPathCollected().split(",");
    QString value = myth->recordData->get("amplitude").toString();
    QStringList valueRowList = value.split(";");
//    qDebug()<<"valueRowList.length()"<<valueRowList.length();
    for(int i = 0;i<valueRowList.length()-1;i++)
    {
        QString str = listRecordPath.at(i+1);
        qDebug()<<valueRowList.at(i);//value从0开始
        if(str.contains(label->text()))
        {
            QFileInfo fi(str);
            if(fi.exists())
            {
                clipFilePath = str;
                //总时长、剪裁开始的时间节点和剪裁结束的时间节点记录在timelengthlb2中
                timelengthlb2->setText(MainWindow::mutual->playerTotalTime(listRecordPath.at(i+1)));
                //剪裁时试听start
                MainWindow::mutual->mpvPlayer->setMedia(QUrl::fromLocalFile(str));
                MainWindow::mutual->mpvPlayer->setVolume(50);
                stopReplayer();//先暂停再播放
                MainWindow::mutual->mpvPlayer->play();//2020/12/14暂时禁用
                play_pause = true;
                themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
                //剪裁时试听end
                QFile file(str);
                if(fi.suffix().contains("mp3"))
                {
                    time = file.size()/16000;
                    qDebug()<<"文件时长:"<<time;
                }
                else if(fi.suffix().contains("wav"))
                {
                    time = file.size()/96000;
                    qDebug()<<"文件时长:"<<time;
                }
                else
                {
                    time = file.size()/4550;
                    qDebug()<<"文件时长:"<<time;
                }
                movePos = leftBtn->width();
                end_Time = time;//点击剪裁时默认把总时长给剪裁结束的值
                moveTime->start(time*1000/railWid->width());

                //qDebug()<<"第:"<<i+1<<"个";
                QString str = valueRowList.at(i);
                valueList = str.split(",");
                //qDebug()<<valueList.length();
                for(int j = 0;j<valueList.length()-1;j++)
                {
                    QString str2=valueList.at(j);
                    array[j]=str2.toInt();
                }
                //amplitudeNum.clear();
                for(int i=0;i<ITEMWINDOW_RECTANGLE_COUNT;i++)
                {
//                     放大检测数值的大小便于观察，
//                     方便显示在剪裁波形图的框里否则对于很小的数可能会显示空白
                    if(array[i]<=500)
                    {
                        mywave.at(i)->setValue(5);
                    }
                    else if(array[i]>500&&array[i]<=1500)
                    {
                        mywave.at(i)->setValue(10);
                    }
                    else
                    {
                        mywave.at(i)->setValue(array[i]/300);
                    }
                }
            }
            else
            {
                QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                                     tr("The file path does not exist or has been deleted!"));
                myth->deleteLater();
                return -1;
            }
        } 

    }
    myth->deleteLater();
    return 0;
}

void ItemsWindow::cursorMove()
{
    //qDebug()<<"游标移动"<<movePos;
    if((movePos<rightBtn->pos().rx()-1)&&(cursorCanMove ==1))
    {
        cursor->move(movePos,0);
        movePos++;
    }
    else
    {
        MainWindow::mutual->mpvPlayer->pause();
        moveTime->stop();
    }
}

void ItemsWindow::cancel()
{
    qDebug()<<"取消";
    cursorCanMove = 0;//已点击取消游标停止
    if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PlayingState)
    {
        MainWindow::mutual->mpvPlayer->pause();
        splitLinestackWid->setCurrentIndex(0);//切换至播放条布局
        clipperstackWid->setCurrentIndex(0);//切换至三个按钮的布局

    }
    else
    {
        clipperstackWid->setCurrentIndex(0);//切换至三个按钮的布局
        stackLayout->setCurrentIndex(0);//切换至显示时间label的布局
        splitLinestackWid->setCurrentIndex(0);//播放条布局切换
    }
    //这种多次使用的后期写到一个函数里
    play_pause = false;
    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
//    deleteWaves();//释放内存

}

void ItemsWindow::finish()
{
    clipperFun();
    cursorCanMove = 0;//已点击完成游标停止
    clipperstackWid->setCurrentIndex(0);//切换至录音按钮stackLayout
    stackLayout->setCurrentIndex(0);//切换至显示时间label的布局
    splitLinestackWid->setCurrentIndex(0);//播放条布局切换
    //这种多次使用的后期写到一个函数里
    play_pause = false;
    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
//    deleteWaves();//释放内存
}

void ItemsWindow::clipperFun()
{
    if(end_Time - start_Time !=time&& end_Time > start_Time)
    {

        QString fileClipPath = QFileInfo(clipFilePath).absolutePath() + "/" +
                QFileInfo(clipFilePath).baseName() + "_clip_" +
                QDateTime::currentDateTime().toString("yyyyMMddhhmmss") +
                "." + QFileInfo(clipFilePath).completeSuffix();
        QString cmd = "ffmpeg -i \"" + clipFilePath + "\" -y -ss " +
                timeEditStartTime + " -to " +
                timeEditEndTime + " -c copy \"" +
                fileClipPath + "\"";
        qDebug() << cmd;
        process = new QProcess;
        process->start(cmd);
        connect(process, SIGNAL(finished(int)), this, SLOT(processFinish(int)), Qt::UniqueConnection);
        QMessageBox::information(NULL, tr("剪裁成功"), fileClipPath);
        qDebug()<<"剪裁完成了"<<clipFilePath<<start_Time<<end_Time;
    }
    else
    {
        qDebug()<<"剪裁开始位置和剪裁结束位置不能是音频开始和结尾且开始结束时间不能相同";
    }

}

void ItemsWindow::processFinish(int x)
{
    qDebug()<<"x = "<<x;
}

void ItemsWindow::pressPausePlayer_Slot()
{
    MainWindow::mutual->mpvPlayer->pause();
}
void ItemsWindow::leftBtn_ReleaseStartPlayer_Slot(int leftButton_absolutePos,int leftButton_rightBorderOppositive,int padding)
{
    if(leftButton_rightBorderOppositive>0&&leftButton_rightBorderOppositive<railWid->width())
    {
        if(padding >0)
        {
            //除去两个滑块所占位置剩余的部分即为轨道,轨道宽度为wave除去两个滑块的宽度
            int currentPlayerPos = static_cast<int>(MainWindow::mutual->mpvPlayer->duration())/railWid->width()*leftButton_rightBorderOppositive;
            qDebug()<<"左滑块右边界相对轨道的位置"<<leftButton_rightBorderOppositive
                    <<"轨道长度:"<<railWid->width()
                    <<"当前播放歌曲的总长度"<<MainWindow::mutual->mpvPlayer->duration()
                    <<"当前播放位置:"<<currentPlayerPos;
            QTime currentTime(0,static_cast<int>(currentPlayerPos) / 60000, static_cast<int>((currentPlayerPos % 60000) / 1000.0));
            QString current_timeStr = currentTime.toString("hh:mm:ss");
            start_Time = currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
            timeEditStartTime = current_timeStr;
            timelengthlb2->setText(current_timeStr);//获取裁剪开始时间
            if (qAbs(MainWindow::mutual->mpvPlayer->position() - currentPlayerPos) > 99)
                MainWindow::mutual->mpvPlayer->setPosition(currentPlayerPos);
            MainWindow::mutual->mpvPlayer->play();
//            qDebug()<<"左按钮右边界相对位置:"<<leftButton_rightBorderOppositive+12;
            movePos = leftButton_rightBorderOppositive+leftBtn->width();//相对位置+按钮宽度=游标的位置
            moveTime->start(time*1000/railWid->width());//左按钮移动后,游标也从左按钮移动的位置处开始

        }
    }
    if(leftButton_absolutePos > rightBtn->pos().rx())
    {
        int currentPos = static_cast<int>(MainWindow::mutual->mpvPlayer->duration())/railWid->width()*(rightBtn->pos().rx()-rightBtn->width()-5);
        QTime currentTime(0,static_cast<int>(currentPos) / 60000, static_cast<int>((currentPos % 60000) / 1000.0));
        QString start_timeStr = currentTime.toString("hh:mm:ss");
        timelengthlb2->setText(start_timeStr);//获取裁剪开始时间=右按钮左边界的相对位置-5
    }
    if(leftButton_absolutePos <= 0)
    {
        timelengthlb2->setText("00:00:00");//获取裁剪开始时间
        start_Time = 0;
        timeEditStartTime = "00:00:00";
    }
}

//右按钮释放获取剪裁结束位置
void ItemsWindow::rightBtn_ReleaseGetEndPositon_Slot(int rightButton_absolutePos,int rightButton_leftBorderOppositive,int padding)
{
    if(rightButton_leftBorderOppositive>0&&rightButton_leftBorderOppositive<railWid->width())
    {
        if(padding >= 0)
        {
            int currentPos = static_cast<int>(MainWindow::mutual->mpvPlayer->duration())/railWid->width()*rightButton_leftBorderOppositive;
            QTime currentTime(0,static_cast<int>(currentPos) / 60000, static_cast<int>((currentPos % 60000) / 1000.0));
            QString end_timeStr = currentTime.toString("hh:mm:ss");
            end_Time = currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
            timeEditEndTime = end_timeStr;
            timelengthlb2->setText(end_timeStr);//获取裁剪结束时间
            //if (qAbs(player->position() - currentPos) > 99)
            //    player->setPosition(currentPos);
            //player->play();
        }
        else
        {
            qDebug()<<"内边距为负值!";
        }
    }
    //如果绝对位置>轨道长度,则剪裁结束时间为文件总时长
    if(rightButton_absolutePos>=railWid->width())
    {
        QTime totalTime(0,(MainWindow::mutual->mpvPlayer->duration()/60000) % 60,
                       (MainWindow::mutual->mpvPlayer->duration() / 1000) % 60);
        timelengthlb2->setText(totalTime.toString("hh:mm:ss"));//获取裁剪结束时间
        end_Time = totalTime.hour()*3600+totalTime.minute()*60+totalTime.second();
        timeEditEndTime = totalTime.toString("hh:mm:ss");
    }
    //如果绝对位置<0,则剪裁结束时间为
    if(rightButton_absolutePos < 0||padding < 0)
    {
        int currentPos = static_cast<int>(MainWindow::mutual->mpvPlayer->duration())/railWid->width()*(leftBtn->pos().rx()+5);
        QTime currentTime(0,static_cast<int>(currentPos) / 60000, static_cast<int>((currentPos % 60000) / 1000.0));
        QString end_timeStr = currentTime.toString("hh:mm:ss");
        timelengthlb2->setText(end_timeStr);//获取裁剪结束时间=左按钮右边界的相对位置+5
        qDebug()<<"左按钮右边界的相对位置+5:"<<leftBtn->pos().rx()+5;
    }

}

void ItemsWindow::leftButton_rightBorderSlot(int x)
{
    if(MainWindow::mutual->mpvPlayer->state() == MMediaPlayer::PlayingState)
    {
       qDebug()<<"试听位置(左滑块右边界相对振幅图位置):"<<x<<"当前播放歌曲的总长度"<<MainWindow::mutual->mpvPlayer->duration();
    }  
}

void ItemsWindow::initRectangleWave()//初始化矩形框
{   

    if(mywave.count() == 0)
    {
        for (int i=0;i<ITEMWINDOW_RECTANGLE_COUNT;i++)//频率直方图
        {
//            myWave *wave=new myWave(this);//每次都要初始化一个矩形框
            wave=new myWave(this);
            wave->setRange(0,100);
            mywave.append(wave);
            waveLayout->addWidget(wave);
        }
        qDebug()<<"初始化矩形框"<<mywave.count();
    }
    waveLayout->setMargin(2);
    waveLayout->setSpacing(1);
    railWid->setLayout(waveLayout);

    rectangleLayout->addWidget(railWid,0,Qt::AlignCenter);
    rectangleLayout->setMargin(0);
    railWid->setFixedSize(387,33);//轨道大小
    railWid->setStyleSheet("background-color:pink;");

    cutWaveWid->setLayout(rectangleLayout);
    cutWaveWid->setFixedSize(427,35);//包含滑块和轨道在内的矩形框大小
    cutWaveWid->setObjectName("cutWaveWid");//设置命名空间
    cutWaveWid->setStyleSheet("#cutWaveWid{border:1px solid #F94974;}");


    leftBtn->setParent(cutWaveWid);
    rightBtn->setParent(cutWaveWid);
    cursor->setParent(cutWaveWid);
    cursor->setFixedSize(2,35);

    leftBtn->setFixedSize(20,35);
    rightBtn->setFixedSize(20,35);
    leftBtn->setIcon(QIcon(":/svg/svg/left_prev.svg"));
    rightBtn->setIcon(QIcon(":/svg/svg/right_prev.svg"));
    leftBtn->setIconSize(QSize(20,35));//重置图标大小
    rightBtn->setIconSize(QSize(20,35));//重置图标大小
    leftBtn->move(0,0);
    cursor->move(leftBtn->width(),0);
    rightBtn->move(cutWaveWid->width()-leftBtn->width(),0);
    qDebug()<<cutWaveWid->width()<<cutWaveWid->height()<<leftBtn->width();

    leftBtn->setObjectName("leftBtn");
    leftBtn->setStyleSheet("#leftBtn{background-color:#F94974;border:1px solid #F94974;}");
    rightBtn->setObjectName("rightBtn");
    rightBtn->setStyleSheet("#rightBtn{background-color:#F94974;border:1px solid #F94974;}");
    cursor->setObjectName("cursor");
    cursor->setStyleSheet("#cursor{border-radius: 1px; background-color:#141414;}");


}

void ItemsWindow::deleteWaves()
{
    for(myWave *wave :mywave)
    {
        wave->deleteLater();//先delete
    }
    mywave.clear();//后清除
    qDebug()<<"释放内存";
}
