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
    int WIDTH=420;
    int HEIGHT=256;
    mainWid = new QWidget();
    mainWid->setWindowFlag(Qt::Tool);//此代码必须在此窗管协议前，否则此模态窗口背景不变灰
    // 添加窗管协议
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(mainWid->winId(), hints);

    Data = new QGSettings(KYLINRECORDER);
    darkData=new QGSettings(FITTHEMEWINDOW);
    // 用户手册功能
    mDaemonIpcDbus = new DaemonDbus();

    mainWid->setFixedSize(WIDTH,HEIGHT);
    setFocusPolicy(Qt::ClickFocus);//this->setFocusPolicy(Qt::NoFocus);//设置焦点类型
    setWindowTitle(tr("Settings"));
//    mainWid->setWindowIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/svg/svg/recording_128.svg")));
//    QScreen *screen = QGuiApplication::primaryScreen();
//    mainWid ->move((screen->geometry().width() - WIDTH) / 2,(screen->geometry().height() - HEIGHT) / 2);
    //显示在活动屏幕中间新方法

    //标题栏设置和布局
    QLabel *lb=new QLabel(this);
    QPushButton *piclb=new QPushButton(this);//窗体左上角图片Label
    piclb->setIcon(QIcon::fromTheme("kylin-recorder", QIcon(":/png/png/recording_32.png")));
    piclb->setFixedSize(25,25);
    piclb->setIconSize(QSize(25,25));//重置图标大小
    piclb->setStyleSheet("QPushButton{border:0px;background:transparent;}");
    lb->setText(tr("Settings"));
    lb->setStyleSheet("font-size:14px;");


    //关闭按钮
    closeButton = new QToolButton(this);
    closeButton->setFixedSize(30,30);
//    closeButton->setToolTip(tr("Close"));
    closeButton->setIcon(QIcon::fromTheme("window-close-symbolic"));//主题库的叉子图标
    closeButton->setProperty("isWindowButton", 0x2);
    closeButton->setProperty("useIconHighlightEffect", 0x8);
    closeButton->setAutoRaise(true);

    alterBtn = new QPushButton(this);
    alterBtn->setFixedSize(65,36);
    alterBtn->setText(tr("Alter"));


    titleWid = new QWidget();
    titleLayout= new QHBoxLayout();
    titleLayout->addWidget(piclb);
    titleLayout->addWidget(lb);
    titleLayout->addWidget(closeButton);
    titleLayout->setContentsMargins(8,4,4,0);
    titleWid->setFixedHeight(35);
    titleWid->setLayout(titleLayout);

    centerWid = new QWidget();
    fileStoreWid = new QWidget();//文件存储wid
    fileFormatWid = new QWidget();//文件格式wid
    sourceWid = new QWidget();//录音来源wid
    btnWid = new QWidget();//按钮wid
    centerLayout = new QVBoxLayout();
    fileStoreLayout = new QHBoxLayout();//文件存储Layout
    fileFormatLayout = new QHBoxLayout();//文件格式Layout
    sourceLayout = new QHBoxLayout();//录音来源Layout
    btnLayout = new QHBoxLayout();//按钮Layout

    storelabel = new QLabel(tr("Storage:"));
    storelabel->setFixedWidth(60);
    storelabel->setStyleSheet("font-size:14px;");
    defaultLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/";
    pathLabel = new MyLabel(this);
    pathLabel->setStyleSheet("font-size:14px;");

    if(Data->get("path").toString()== ""){
        qDebug()<<"初始配置文件为空自动填充默认路径";
        pathLabel->setText(defaultLocation+tr("Recorder"));
        Data->set("path",defaultLocation);
    }
    else{
        pathLabel->setText(Data->get("path").toString()+tr("Recorder"));
    }
    if(Data->get("recorderpath").toString()== ""){
        Data->set("recorderpath",","+defaultLocation);
    }
    pathLabel->setAcceptDrops(false);//禁止拖拽入
    pathLabel->setFixedSize(209,36);
    fileStoreLayout->addWidget(storelabel);
    fileStoreLayout->addWidget(pathLabel);
    fileStoreLayout->addWidget(alterBtn);
    fileStoreWid->setLayout(fileStoreLayout);

    formatLabel = new QLabel(tr("Format:"));
    formatLabel->setFixedWidth(60);
    formatLabel->setStyleSheet("font-size:14px;");
    formatDownList = new QComboBox(this);
    formatDownList->setStyleSheet("font-size:14px;");
    formatDownList->setFixedSize(280,36);

    formatDownList->addItem(tr("mp3"));
    formatDownList->addItem(tr("m4a"));
    formatDownList->addItem(tr("wav"));
    fileFormatLayout->addWidget(formatLabel);
    fileFormatLayout->addWidget(formatDownList,0,Qt::AlignLeft);
    fileFormatWid->setLayout(fileFormatLayout);

    sourceLabel = new QLabel(tr("Source:"));
    sourceLabel->setFixedWidth(60);
    sourceLabel->setStyleSheet("font-size:14px;");
    sourceDownList = new QComboBox(this);
    sourceDownList->setStyleSheet("font-size:14px;");
    sourceDownList->setFixedSize(280,36);
    sourceDownList->addItem(tr("Microphone"));
//    sourceDownList->addItem(tr("System Inside"));

    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(sourceDownList,0,Qt::AlignLeft);
    sourceWid->setLayout(sourceLayout);

    centerLayout->addWidget(fileStoreWid);
    centerLayout->addWidget(fileFormatWid);
    centerLayout->addWidget(sourceWid);
    centerLayout->setContentsMargins(32,16,24,24);
    centerWid->setLayout(centerLayout);

    connect(closeButton,&QToolButton::clicked,mainWid,&Settings::close);
    connect(alterBtn,&QPushButton::clicked,this,&Settings::openFileDialog);
//    connect(storeEdit,&LineEdit::clicked,this,&Settings::openFileDialog);
//    connect(storeEdit,&LineEdit::textChanged,this,&Settings::inputEditJudge);
//    connect(storeEdit,&LineEdit::textEdited,this,&Settings::editSlot);
    connect(formatDownList,SIGNAL(currentIndexChanged(int)),this,SLOT(saveType(int)));
    connect(sourceDownList,SIGNAL(currentIndexChanged(int)),this,SLOT(recordSource(int)));

    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleWid);
    mainLayout->addWidget(centerWid,0,Qt::AlignHCenter);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    mainWid->setLayout(mainLayout);
    mainWid->setAttribute(Qt::WA_ShowModal, true);//模态窗口

    if(Data->get("source").toInt() == 1)
    {
//        qDebug()<<"麦克风";
        sourceDownList->setCurrentIndex(0);
    }
    else if(Data->get("source").toInt() == 2)//内部
    {
        qDebug()<<"内部";
    }
    else if(Data->get("source").toInt() == 3)//麦克风
    {
        qDebug()<<"全部";
    }
    else
    {

    }

    if(Data->get("type").toInt() == 1)//开始录制格式为mp3
    {
        formatDownList->setCurrentIndex(0);
    }
    else if(Data->get("type").toInt() == 2)//开始录制格式为m4a
    {
        formatDownList->setCurrentIndex(1);
    }
    else if(Data->get("type").toInt() == 3)//开始录制格式为wav
    {
        formatDownList->setCurrentIndex(2);
    }

}

// 实现键盘响应
void Settings::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"键盘事件";
    // F1快捷键打开用户手册
    if (event->key() == Qt::Key_F1) {
        if (!mDaemonIpcDbus->daemonIsNotRunning()){
            //F1快捷键打开用户手册，如kylin-recorder
            //由于是小工具类，下面的showGuide参数要填写"tools/kylin-recorder"
            mDaemonIpcDbus->showGuide("tools/kylin-recorder");
        }
    }
}

//打开文件目录
void Settings::openFileDialog()
{
//这个是选择保存多文件格式的
//    QString selectedFilter;
//    QString fileType = "Mp3(*.mp3);;M4a(*.m4a);;Wav(*.wav)";
//    fileName = QFileDialog::getSaveFileName(
//                      mainWid,
//                      tr("Select a file storage directory"),
//                          QDir::currentPath(),
//                          fileType,
//                          &selectedFilter);
    //选择文件目录
    selectDirPath = QFileDialog::getExistingDirectory(
                            mainWid,
                            tr("Select a file storage directory"),
                            "/");
    qDebug()<<"选择的文件目录是:"<<selectDirPath;
    int value = inputEditJudge(selectDirPath+"/");
    if(value == 1 ){
        //正常时选择目录,要把目录集写入配置文件recorderpath中
        pathLabel->setText(selectDirPath+"/"+tr("Recorder"));
        QString strSaveFilePath = Data->get("recorderpath").toString();
        if(strSaveFilePath!=""){
            QString newFilePath=strSaveFilePath+","+selectDirPath+"/";
            QStringList pathList = strSaveFilePath.split(",");
            if(!pathList.contains(selectDirPath+"/")){
                Data->set("recorderpath",newFilePath);
            }else{
                qDebug()<<"包含路径就不用写了";
            }

        }
        qDebug()<<"查看一下存入的路径:"<<Data->get("recorderpath").toString();

    }else if (value == 0 ){
        //命名非法时选择默认路径
        Data->set("path",defaultLocation);
        pathLabel->setText(defaultLocation+tr("Recorder"));
    }else{
        //为空时选择默认路径
        Data->set("path",Data->get("path").toString());
        pathLabel->setText(Data->get("path").toString()+tr("Recorder"));
    }

}

//编辑label时判断
int Settings::inputEditJudge(QString fileDir)
{
    QString str = fileDir;
    if(fileDir != "/"){
        qDebug()<<"输入的文件名是:"<<fileDir;
        //子串中至少包含"/home/用户名/"
        QString subStr = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/";
        QString path = str.mid(0,str.lastIndexOf("/"));
        QFileInfo fi(path);
        qDebug()<<"满足条件的前提"<<subStr<<"输入的是:"<<str<<"最后一个/之前的路径:"<<path;
        if(str != "")
        {
            if(!str.contains(subStr)||str.contains(".")||str.contains(";")||
                    str.contains("#")||str.contains("?")||str.contains(" ")||
                   str.contains("\"")||str.contains("'")||str.contains("\\"))
            {
                //主要是防止非法路径头:至少包含"/home/用户名/"
                QMessageBox::warning(mainWid,tr("Warning"),tr("This storage path is illegal!"));
                return 0;
            }
            else
            {
                if(fi.exists())
                {
                    QString name = str.split("/").last();
                    int length = name.length();
                    qDebug()<<"长度:"<<length;
                    if(length<=20){
                        Data->set("path",fileDir);
                        return 1;
                    }else{
                        //超过20个字符就提示命名不能超过20个字符
                        QMessageBox::warning(mainWid,tr("Warning"),tr("The file name cannot exceed 20 characters!"));
                        return 0;
                    }

                }
                else
                {
                    //主要是防止非法路径尾,比如：用户自己乱写的目录/home/bjc/音乐/h/h显然此路径不存在
                    QMessageBox::warning(mainWid,tr("Warning"),tr("This storage path is illegal!"));
                    return 0;
                }

            }

        }
        else//当点击关闭时storeEdit会为空
        {
            qDebug()<<"若为空则还是选择之前的路径";
            return 2;
        }
    }
    return 2;

}

//要提示用户不可以在QLineEdit这里修改路径，只能去规定的弹窗中修改防止用户乱改!!!
void Settings::editSlot()
{
    qDebug()<<"文本框编辑";
//    storeEdit->setText(defaultLocation);
//    QMessageBox::warning(mainWid,tr("Warning"),tr("You cannot edit here!"));
    return ;
}

void Settings::saveType(int index)
{
    if(index == 0){
        Data->set("type",1);//mp3
        qDebug()<<"类型改为"<<Data->get("type").toInt();
    }else if(index == 1){
        Data->set("type",2);//m4a
        qDebug()<<"类型改为"<<Data->get("type").toInt();
    }else if(index == 2){
        Data->set("type",3);//wav
        qDebug()<<"类型改为"<<Data->get("type").toInt();
    }else{

    }
}

void Settings::recordSource(int index)//mic录音源
{
    if(index ==0){
        qDebug()<<"设置录音源为mic";
        Data->set("source",1);
    }else if(index ==1){
        qDebug()<<"设置录音源为系统内部";
    }else{

    }

}

