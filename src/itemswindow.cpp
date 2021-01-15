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
    playList = new QMediaPlaylist;//播放列表
    player = new QMediaPlayer;//媒体文件
    line = new QFrame();//分割线

    playSlider = new QSlider(this);//播放滑动条
    playSlider->installEventFilter(this);//给playSlider安装事件过滤器
    listNum = new QLabel(this);//录音标号标签
    listNumChangeColor = new QLabel(this);//颜色
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

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));

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
    playSlider->setFixedHeight(3);//后期要改成3
    playSlider->setStyleSheet("QSlider::handle:horizontal{width:2px;background-color:#3790FA;margin:-5px 0px -5px 0px;border-radius:1px;}"
                              "QSlider::groove:horizontal{height:3px;background-color:#EDEDED;}"
                              "QSlider::add-page:horizontal{background-color:#EDEDED;}"
                              "QSlider::sub-page:horizontal{background-color:#3790FA;}");

    listNum->setObjectName("listNum");
    listNum->setStyleSheet("font-size:14px;");//录音序号

    recordFileName->setObjectName("dateTimelb");//设置对象名
    recordFileName->setStyleSheet("font-size:14px;");//录音文件名
    testlb->setText(" ");
    timelengthlb->setText("00:00:00");
    itemPlay_PauseButton->setFixedSize(32,32);
    itemPlay_PauseButton->setToolTip(tr("play/pause"));

    clipButton->setFixedSize(32,32);
    clipButton ->setToolTip(tr("clip"));

    deleteButton ->setFixedSize(32,32);
    deleteButton ->setToolTip(tr("delete"));
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color:#EDEDED;");

    fileName_dateTimeLayout->addWidget(listNum);
    fileName_dateTimeLayout->addWidget(recordFileName);
    fileName_dateTimeLayout->setSpacing(0);
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

    itemBottomLayout->addWidget(fileName_dateTimeWid);//录音序号与文件名的布局
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
    clipperstackWid -> addWidget(clipperWid);

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
        listNum->setStyleSheet("font-size:14px;color:white;");
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
        listNum->setStyleSheet("font-size:14px;color:black;");
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
    if(player->state() == QMediaPlayer::PlayingState)
    {
        playSlider->setValue(static_cast<int>(position));
        QTime currentTime(0, static_cast<int>(position) / 60000, static_cast<int>((position % 60000) / 1000.0));
        QString current_timeStr = currentTime.toString("hh:mm:ss");
        QTime totalTime(0,(player->duration()/60000) % 60,
                       (player->duration() / 1000) % 60);
        qDebug()<<current_timeStr + "/" + totalTime.toString("hh:mm:ss");
//        liveTime->setText(current_timeStr);
//        if (position > player->duration()) {
//            qDebug()<<"列表播放结束 停止" ;
//            player->stop();
//        }
        if(current_timeStr == totalTime.toString("hh:mm:ss")&&current_timeStr!="00:00:00")//如果播放完毕则自动停止
        {
            qDebug()<<"列表播放结束 停止" ;
            isPlayerEnd = 1;
            player->stop();
            play_pause=false;

            themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
            setPosition(0);
            emit playingSignal(false);//播放结束才可以点击录音
//            stackLayout -> setCurrentIndex(0);//3个按钮布局切换至显示时间label
//            splitLinestackWid -> setCurrentIndex(0);//播放条布局切换
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
            qDebug()<<"左键";
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
    else if(obj == itemsWid)//悬浮特效、点击特效、右击特效
    {
        itemsWid->setAttribute(Qt::WA_Hover,true);//****关键代码,若不加此行代码则没有悬浮特效****
        listNumChangeColor = itemsWid->findChild<QLabel *>(listNum->objectName());
        recordFileNameChangeColor = itemsWid->findChild<QLabel *>(recordFileName->objectName());
        if(event->type() == QEvent::HoverEnter)//显示浮窗
        {
            //qDebug()<<"进入";
            listNumChangeColor->setStyleSheet("font-size:14px;color:#3790FA;");
            recordFileNameChangeColor->setStyleSheet("font-size:14px;color:#3790FA;");
            int nIndex = stackLayout->currentIndex();
            nIndex++;
            stackLayout->setCurrentIndex(1);//切换至录音按钮stackLayout
        }
        else if(event->type() == QEvent::HoverLeave)//收起浮窗
        {
            //qDebug()<<"离开";
            //此处为鼠标悬停离开某item时,字体颜色变化要注意和主题对应黑主题白字白主题黑字
            if(MainWindow::mutual->themeData->get("style-name").toString() == "ukui-dark"||MainWindow::mutual->themeData->get("style-name").toString() == "ukui-black")
            {
                listNumChangeColor->setStyleSheet("font-size:14px;color:white;");
                recordFileNameChangeColor->setStyleSheet("font-size:14px;color:white;");
            }
            else
            {
                listNumChangeColor->setStyleSheet("font-size:14px;color:black;");
                recordFileNameChangeColor->setStyleSheet("font-size:14px;color:black;");
            }

            stackLayout->setCurrentIndex(0);//切换至录音按钮stackLayout
            splitLinestackWid->setCurrentIndex(0);//切换至分割线
            if(player->state() == QMediaPlayer::PlayingState)
            {
                splitLinestackWid->setCurrentIndex(1);//切换至进度条
            }
            else
            {
                 splitLinestackWid->setCurrentIndex(0);
            }

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
//    menu->addAction(actionOpenFolder);//2020/12/12禁用
    menu->exec(QCursor::pos());

    menu->deleteLater();
    actionSave->deleteLater();
    actionOpenFolder->deleteLater();
}

void ItemsWindow::actionSaveasSlot()
{
    MyThread *my = new MyThread;//要用MyThread类的系统弹出框，由于ItemsWindowd的弹出框样式变了
    QLabel *label = itemsWid->findChild<QLabel *>(recordFileName->objectName());
    QStringList listRecordPath = my->readPathCollected().split(",");//先读取配置文件中的所有路径
    int m = my->readNumList()-1;
    for(int i=1;i<=m;i++)
    {
        QString str=listRecordPath.at(i);
        if(str.contains(label->text()))
        {
            QFileInfo fi(str);
            if(fi.exists())
            {
                my->saveAs(str);
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
    my->deleteLater();
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
    play_pause = true;
    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标
}
void ItemsWindow::playState()//播放状态
{
    if(player->state()==QMediaPlayer::PlayingState)
    {
        qDebug()<<"有正在播放的音频";
        player->stop();
        stackLayout->setCurrentIndex(0);//切换至录音按钮stackLayout
        splitLinestackWid->setCurrentIndex(0);//2020.11.12隐藏此滑动条功能
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
        player->stop();
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
//    if(MainWindow::isRecording)
//        return;
emit playingSignal(true);
    MyThread *myth = new MyThread();
    QLabel *label = itemsWid->findChild<QLabel *>(recordFileName->objectName());
    myth->readPathCollected();//先读取配置文件中的所有路径集

    QStringList listRecordPath = myth->readPathCollected().split(",");
    int m = myth->readNumList()-1;
    for(int i=1;i<=m;i++)
    {
        audioFilePath = listRecordPath.at(i);
        if(audioFilePath.contains(label->text()))
        {
            QFileInfo fi(audioFilePath);
            if(fi.exists())
            {

                if(player->state() == QMediaPlayer::PlayingState)
                {

                    qDebug()<<"有正在播放的音频";
                    player->pause();
                    qDebug()<<"存在暂停"<<label->text();
                    play_pause=false;
                    emit playingSignal(false);//点击暂停之后才可以再点击录音
                    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标

                }
                else
                {

                    if(!isOpen)
                    {
                        player -> setMedia(QUrl::fromLocalFile(audioFilePath ));
                        isOpen = true;
                        emit playingSignal(true);
                    }
                    splitLinestackWid->setCurrentIndex(1);//点击播放时在显示那个进度条
                    player->setVolume(50);
                    stopReplayer();//先暂停再播放
                    player->play();
                    qDebug()<<"存在播放"<<label->text();
                    play_pause = true;
                    themeStyle(MainWindow::mutual->themeData->get("style-name").toString());//根据主题变换播放暂停图标

                }
            }
            else
            {
                emit playingSignal(false);
                isExistAudioFile(audioFilePath);//看是否已存在音频文件
                updateGSetting(audioFilePath);//更新配置文件,一定要加上
                WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                         ,tr("The file path does not exist or has been deleted!"),QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
                return ;
            }
        }
    }

}

void ItemsWindow::isExistAudioFile(QString fileName)
{
    qDebug()<<fileName<<"已经不存在！！,要自动删除之后还要更新配置文件";
    MyThread *myth = new MyThread();//构造函数实例化后构造函数被调用。recordPathData在MyThread的构造里面
    QStringList listRecordPath = myth->readPathCollected().split(",");
    QStringList listAmplitude = myth->recordData->get("amplitude").toString().split(";");
    int m = myth->readNumList();//因为配置文件初始为1
    if(m<0)
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                 ,tr("The current number of list files is 0."),QMessageBox::Yes );
        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
        WrrMsg->exec();
        return ;
    }
    for(int i = 0; i<m; i++)
    {
        QString str = listRecordPath.at(i);
        //QString strTemp = listAmplitude.at(i-1);
        if(str.contains(fileName))
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
                    QString oldAmplitudeStr = myth->recordData->get("amplitude").toString();
                    int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);
                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    myth->writePathCollected(newStr);
                    QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
                    myth->recordData->set("amplitude",newAmplitudeStr);
                    myth->writeNumList(myth->readNumList()-1);
                    //根据索引值删除listwidget列表的Item，要注意配置文件的更新以及本地文件的删除
//                    int index=MainWindow::mutual->list->currentRow();
                    this->deleteLater();//先释放内存再删除列表的项,要成对出现
                    MainWindow::mutual->list->takeItem(i-1);//删除操作
                    //传item个数,如果为0则显示"文件列表为空"标签
                    MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());
                    QFile::remove(str);//从本地删除
                }
                else
                {
                    qDebug()<<"文件不存在!或已经被删除!";
                }
            }
            else
            {
                //本地文件已经被删除时，删除按钮就直接删除listwidget的item项
                this->deleteLater();//先释放内存再删除列表的项,要成对出现
                MainWindow::mutual->list->takeItem(i-1);//删除list列表的item操作
                MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());//传item个数
                break ;
            }

        }
    }
    return ;
}

//更新配置文件,
void ItemsWindow::updateGSetting(QString fileName)
{
    int  m=itemData->get("num").toInt();
    qDebug()<<"ssssssssssssss"<<m;
    if(m == 1)
    {
        MainWindow::mutual->isFileNull(m-1);
    }
    //qDebug()<<m;
    QStringList listRecordPath = itemData->get("recorderpath").toString().split(",");
    qDebug()<<listRecordPath;
    QStringList listAmplitude = itemData->get("amplitude").toString().split(";");
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
            //判断文件路径是否存在,且不重复
            if(fileinfo.isFile()&&(str!=fileName)&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
            {
                qDebug()<<"文件存在!且不重复";
                //1.每当配置文件中有路径时就在list中更新一下,1必须在2、3前面先更新后删除
                MainWindow::mutual->slotListItemAdd(str,i);
                //2.先释放内存再删除列表的项,要成对出现
                this->deleteLater();
                //3.删除list列表的item操作
                MainWindow::mutual->list->takeItem(i-1);

            }
            else
            {
                qDebug()<<"文件存在!但是已经重复!!!!";
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
            QString oldStr=itemData->get("recorderpath").toString();
            int pos=oldStr.indexOf(subStr);
            QString oldAmplitudeStr = itemData->get("amplitude").toString();
            int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);
            //qDebug()<<pos<<" "<<oldStr;
            //qDebug()<<oldStr.mid(pos,str.length()+1);
            QString newStr = oldStr.remove(pos,str.length()+1);
            itemData->set("recorderpath",newStr);
            QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
            itemData->set("amplitude",newAmplitudeStr);
            itemData->set("num",itemData->get("num").toInt()-1);
//            myThread->writeNumList(myThread->readNumList()-1);
            qDebug()<<itemData->get("recorderpath").toString();
        }

    }
}

void ItemsWindow::delFile()//删除本地音频文件
{
    MyThread *myth = new MyThread();//构造函数实例化后构造函数被调用。recordPathData在MyThread的构造里面
    QLabel *label = itemsWid->findChild<QLabel *>(recordFileName->objectName());
    myth->readPathCollected();//先读取配置文件中的所有路径集
    QStringList listRecordPath = myth->readPathCollected().split(",");
    QStringList listAmplitude = myth->recordData->get("amplitude").toString().split(";");
    int m = myth->readNumList();//因为配置文件初始为1
    if(m<0)
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                 ,tr("The current number of list files is 0."),QMessageBox::Yes );
        WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
        WrrMsg->exec();
        return ;
    }
    for(int i = 0; i<m; i++)
    {
        QString str = listRecordPath.at(i);
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
                    QString oldAmplitudeStr = myth->recordData->get("amplitude").toString();
                    int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);
                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    myth->writePathCollected(newStr);
                    QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
                    myth->recordData->set("amplitude",newAmplitudeStr);
                    myth->writeNumList(myth->readNumList()-1);
                    //根据索引值删除listwidget列表的Item，要注意配置文件的更新以及本地文件的删除
//                    int index=MainWindow::mutual->list->currentRow();
                    this->deleteLater();//先释放内存再删除列表的项,要成对出现
                    MainWindow::mutual->list->takeItem(i-1);//删除操作
                    MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());//传item个数
                    QFile::remove(str);//从本地删除
                }
                else
                {
                    qDebug()<<"文件不存在!或已经被删除!";
                }
            }
            else
            {
                //本地文件已经被删除时，删除按钮就直接删除listwidget的item项
                this->deleteLater();//先释放内存再删除列表的项,要成对出现
                MainWindow::mutual->list->takeItem(i-1);//删除list列表的item操作
                MainWindow::mutual->isFileNull(MainWindow::mutual->list->count());//传item个数
                break ;
            }

        }
    }
    return ;
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
                player->setMedia(QUrl::fromLocalFile(str));
                player->setVolume(50);
                stopReplayer();//先暂停再播放
                player->play();//2020/12/14暂时禁用
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
                WrrMsg = new QMessageBox(QMessageBox::Warning,tr("Warning")
                                         ,tr("The file path does not exist or has been deleted!"),QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
                return -1;
            }
        } 

    }

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
        player->pause();
        moveTime->stop();
    }
}

void ItemsWindow::cancel()
{
    qDebug()<<"取消";
    cursorCanMove = 0;//已点击取消游标停止
    if(player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
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
    player->pause();
}
void ItemsWindow::leftBtn_ReleaseStartPlayer_Slot(int leftButton_absolutePos,int leftButton_rightBorderOppositive,int padding)
{
    if(leftButton_rightBorderOppositive>0&&leftButton_rightBorderOppositive<railWid->width())
    {
        if(padding >0)
        {
            //除去两个滑块所占位置剩余的部分即为轨道,轨道宽度为wave除去两个滑块的宽度
            int currentPlayerPos = static_cast<int>(player->duration())/railWid->width()*leftButton_rightBorderOppositive;
            qDebug()<<"左滑块右边界相对轨道的位置"<<leftButton_rightBorderOppositive
                    <<"轨道长度:"<<railWid->width()
                    <<"当前播放歌曲的总长度"<<player->duration()
                    <<"当前播放位置:"<<currentPlayerPos;
            QTime currentTime(0,static_cast<int>(currentPlayerPos) / 60000, static_cast<int>((currentPlayerPos % 60000) / 1000.0));
            QString current_timeStr = currentTime.toString("hh:mm:ss");
            start_Time = currentTime.hour()*3600+currentTime.minute()*60+currentTime.second();
            timeEditStartTime = current_timeStr;
            timelengthlb2->setText(current_timeStr);//获取裁剪开始时间
            if (qAbs(player->position() - currentPlayerPos) > 99)
                player->setPosition(currentPlayerPos);
            player->play();
//            qDebug()<<"左按钮右边界相对位置:"<<leftButton_rightBorderOppositive+12;
            movePos = leftButton_rightBorderOppositive+leftBtn->width();//相对位置+按钮宽度=游标的位置
            moveTime->start(time*1000/railWid->width());//左按钮移动后,游标也从左按钮移动的位置处开始

        }
    }
    if(leftButton_absolutePos > rightBtn->pos().rx())
    {
        int currentPos = static_cast<int>(player->duration())/railWid->width()*(rightBtn->pos().rx()-rightBtn->width()-5);
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
            int currentPos = static_cast<int>(player->duration())/railWid->width()*rightButton_leftBorderOppositive;
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
        QTime totalTime(0,(player->duration()/60000) % 60,
                       (player->duration() / 1000) % 60);
        timelengthlb2->setText(totalTime.toString("hh:mm:ss"));//获取裁剪结束时间
        end_Time = totalTime.hour()*3600+totalTime.minute()*60+totalTime.second();
        timeEditEndTime = totalTime.toString("hh:mm:ss");
    }
    //如果绝对位置<0,则剪裁结束时间为
    if(rightButton_absolutePos < 0||padding < 0)
    {
        int currentPos = static_cast<int>(player->duration())/railWid->width()*(leftBtn->pos().rx()+5);
        QTime currentTime(0,static_cast<int>(currentPos) / 60000, static_cast<int>((currentPos % 60000) / 1000.0));
        QString end_timeStr = currentTime.toString("hh:mm:ss");
        timelengthlb2->setText(end_timeStr);//获取裁剪结束时间=左按钮右边界的相对位置+5
        qDebug()<<"左按钮右边界的相对位置+5:"<<leftBtn->pos().rx()+5;
    }

}

void ItemsWindow::leftButton_rightBorderSlot(int x)
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
       qDebug()<<"试听位置(左滑块右边界相对振幅图位置):"<<x<<"当前播放歌曲的总长度"<<player->duration();
    }  
}

void ItemsWindow::initRectangleWave()//初始化矩形框
{   

    if(mywave.count() == 0)
    {
        for (int i=0;i<ITEMWINDOW_RECTANGLE_COUNT;i++)//频率直方图
        {
            myWave *wave=new myWave(this);//每次都要初始化一个矩形框
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


