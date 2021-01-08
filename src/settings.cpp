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
#include <QLabel>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QtCore/QCoreApplication>

#include "settings.h"
#include "mainwindow.h"
#include "xatom-helper.h"



Settings::Settings(QWidget *parent) : QMainWindow(parent)
{

    // 添加窗管协议
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);

    Data = new QGSettings(KYLINRECORDER);
    darkData=new QGSettings(FITTHEMEWINDOW);
    // 用户手册功能
    mDaemonIpcDbus = new DaemonDbus();
    int WIDTH=360;
    int HEIGHT=300;
    this->setFixedSize(WIDTH,HEIGHT);
    setFocusPolicy(Qt::ClickFocus);//this->setFocusPolicy(Qt::NoFocus);//设置焦点类型
    setWindowTitle(tr("kylin-recorder"));
    this->setWindowIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/svg/svg/recording_128.svg")));
    //标题栏设置和布局
    QLabel *lb=new QLabel(this);
    QLabel *piclb=new QLabel(this);//窗体左上角图片Label

    piclb->setStyleSheet("QLabel{border-image: url(:/png/png/recording_32.png);}");
    piclb->setFixedSize(25,25);
    lb->setText(tr("Settings"));//

    closeButton= new QToolButton(this);
    closeButton->setIcon(QIcon(":/svg/svg/window-close.svg"));
    closeButton->setStyleSheet("background-color:transparent");
    closeButton->setFixedSize(30,30);
    closeButton->setStyleSheet("QToolButton{border-radius:4px;}"
                               "QToolButton:hover{background-color:#F86457}"
                               "QToolButton:pressed{background-color:#E44C50}");
    closeButton->setProperty("isWindowButton", 0x2);
    closeButton->setProperty("useIconHighlightEffect", 0x8);
    closeButton->setAutoRaise(true);
    connect(closeButton,&QToolButton::clicked,this,&Settings::close);


    titleWid = new QWidget();
    titleLayout= new QHBoxLayout();
    titleLayout->addWidget(piclb);
    titleLayout->addWidget(lb);
    titleLayout->addWidget(closeButton);
    titleLayout->setContentsMargins(8,4,4,0);

    titleWid->setFixedHeight(35);
    titleWid->setLayout(titleLayout);

    //网格布局设置界面主体内容
    //        QGridLayout *layout = new QGridLayout();
    //设置界面内容
    label = new QLabel(tr("Storage"));
    radioButton = new QRadioButton(this);
    //    radioButton->setCheckable(1);
    //radioButton->setChecked(1);//默认选中状态

    connect(radioButton,&QRadioButton::clicked,this,&Settings::gotosave);

    label_2 = new QLabel(tr("Save as"));
    radioButton_2 = new QRadioButton(this);
    //    connect(radioButton_2,&QRadioButton::clicked,this,&subMainWindow::userdirectory);
    connect(radioButton_2,&QRadioButton::clicked,this,&Settings::closesave);

    label_3 = new QLabel(tr("Default storage："));
    label_3->setFixedWidth(120);
    lineEdit = new QLineEdit(this);
    defaultLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    lineEdit->setText(defaultLocation);
    lineEdit->setFocusPolicy(Qt::NoFocus);//设置不可编辑状态
    label_4 = new QLabel(tr("Source"));
    radioButton_3 = new QRadioButton(this);
    label_5 = new QLabel(tr("All"));
    radioButton_4 = new QRadioButton();
    label_6 = new QLabel(tr("Inside"));
    radioButton_5 = new QRadioButton(this);
    label_7 = new QLabel(tr("Microphone"));
    label_8 = new QLabel(tr("File format"));

    radioButton_6 = new QRadioButton(this);
    label_9 = new QLabel("mp3");
    radioButton_7 = new QRadioButton();
    label_10 = new QLabel("m4a");
    radioButton_8 = new QRadioButton(this);
    label_11 = new QLabel("wav");
    connect(radioButton_6,&QRadioButton::clicked,this,&Settings::saveTypeMp3);
    connect(radioButton_7,&QRadioButton::clicked,this,&Settings::saveTypeM4a);
    connect(radioButton_8,&QRadioButton::clicked,this,&Settings::saveTypeWav);
    connect(radioButton_3,&QRadioButton::clicked,this,&Settings::allAudio);
    connect(radioButton_4,&QRadioButton::clicked,this,&Settings::inside);
    connect(radioButton_5,&QRadioButton::clicked,this,&Settings::microphone);

    //按钮分组，网格布局
    QButtonGroup *grp1=new QButtonGroup(this);
    grp1->addButton(radioButton);
    grp1->addButton(radioButton_2);
    QButtonGroup *grp2=new QButtonGroup(this);
    grp2->addButton(radioButton_3);
    grp2->addButton(radioButton_4);//系统内部声音
    grp2->addButton(radioButton_5);
    QButtonGroup *grp3=new QButtonGroup(this);
    grp3->addButton(radioButton_6);
    grp3->addButton(radioButton_7);//m4a格式
    grp3->addButton(radioButton_8);

    //布局
    mainWid = new QWidget(this);
    Wid = new QWidget();
    QGridLayout *pg=new QGridLayout();
    pg->addWidget(label,0,0,1,1);
    pg->addWidget(radioButton,0,2,1,1);
    pg->addWidget(label_2,0,3,1,1);
    pg->addWidget(radioButton_2,1,2,1,1);
    pg->addWidget(label_3,1,3,1,1);
    pg->addWidget(lineEdit,2,3,1,6);
    pg->addWidget(label_4,3,0,1,2);
    //        pg->addWidget(radioButton_3,3,2,1,1);
    //        pg->addWidget(label_5,3,3,1,1);
    //        pg->addWidget(radioButton_4,3,4,1,1);//系统内部解封后改为3,4,1,1
    //        pg->addWidget(label_6,3,5,1,1);//系统内部解封后改为3,5,1,1
    pg->addWidget(radioButton_5,3,2,1,1);//解封后改为3611
    pg->addWidget(label_7,3,3,1,1);//解封后改为3711
    pg->addWidget(label_8,4,0,1,2);
    pg->addWidget(radioButton_6,4,2,1,1);//mp3
    pg->addWidget(label_9,4,3,1,1);
    pg->addWidget(radioButton_7,4,4,1,1);//M4a格式
    pg->addWidget(label_10,4,5,1,1);
    pg->addWidget(radioButton_8,4,6,1,1);//wav,后期改为4611
    pg->addWidget(label_11,4,7,1,1);//后期改为4711
    Wid->setLayout(pg);

    mainLayout= new QVBoxLayout();
    mainLayout->addWidget(titleWid);
    mainLayout->addWidget(Wid);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    mainWid->setLayout(mainLayout);
//    mainWid->setStyleSheet("background-color:pink;");
    this->setCentralWidget(mainWid);

    this->setAttribute(Qt::WA_TranslucentBackground);//窗体透明
    this->setStyleSheet("border-radius:6px;" );//主窗体圆角(注意：窗体透明与主窗体圆角要搭配使用否则无效)
    if(darkData->get("style-name").toString()=="ukui-dark"||darkData->get("style-name").toString() == "ukui-black")
    {
        closeButton->setIcon(QIcon(":/svg/svg/dark-window-close.svg"));
        mainWid->setStyleSheet("background-color:#131314;border:1px;");//自定义窗体(圆角+背景色)

    }
    else
    {
        closeButton->setIcon(QIcon(":/svg/svg/window-close.svg"));
        mainWid->setObjectName("setmainWid");//设置命名空间
        mainWid->setStyleSheet("#setmainWid{background-color:#FFFFFF;border:1px solid #262626;}");//自定义窗体(圆角+背景色)
    }


    this->move((QApplication::desktop()->width() -WIDTH)/3, (QApplication::desktop()->height() - HEIGHT)/3);

    if(Data->get("savedefault").toInt()==1)
    {
        radioButton->setChecked(1);
    }
    else
    {
        radioButton_2->setChecked(1);//默认存储
    }

    if(Data->get("source").toInt() == 1)
    {
        radioButton_3->setChecked(1);//开始录制声源为全部
    }
    else if(Data->get("source").toInt() == 2)//内部
    {
        radioButton_4->setChecked(1);
    }
    else if(Data->get("source").toInt() == 3)//麦克风
    {
        radioButton_5->setChecked(1);
    }
    else
    {

    }

    if(Data->get("type").toInt() == 1)//开始录制格式为mp3
    {
        radioButton_6->setChecked(1);
    }
    else if(Data->get("type").toInt() == 2)//开始录制格式为m4a
    {
        radioButton_7->setChecked(1);
    }
    else if(Data->get("type").toInt() == 3)//开始录制格式为wav
    {

        radioButton_8->setChecked(1);
    }

}

// 实现键盘响应
void Settings::keyPressEvent(QKeyEvent *event)
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

void Settings::gotosave()
{
    Data->set("savedefault",1);

}
void Settings::closesave()
{
    Data->set("savedefault",0);   
}
void Settings::saveTypeMp3()//mp3
{
    Data->set("type",1);
}
void Settings::saveTypeM4a()//m4a
{
    Data->set("type",2);
}
void Settings::saveTypeWav()//wav
{
    Data->set("type",3);
}
void Settings::allAudio()//全部
{
    Data->set("source",1);
}
void Settings::inside()//内部声音
{
    Data->set("source",2);
}
void Settings::microphone()//麦克风
{
    Data->set("source",3);
}
