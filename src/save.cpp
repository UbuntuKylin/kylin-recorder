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
#include "save.h"
#include "mainwindow.h"
#include "lineedit.h"
#include <QMessageBox>
#include <QAudioFormat>
#include <QFileDialog>
#include <QAudioOutput>
#include <QDebug>

Save::Save(QWidget *parent) : QMainWindow(parent)
{
    saveData=new QGSettings(KYLINRECORDER);
    //qDebug()<<KYLINRECORDER;
    int WIDTH=360;
    int HEIGHT=300;
    this->setFixedSize(WIDTH,HEIGHT);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("麒麟录音");
    this->setWindowIcon(QIcon(":/svg/svg/recording_128.svg"));
    //setStyleSheet(QString::fromUtf8("border-radius:12px;background-color:#FFFFFF;border-width:1px;"));

    QLabel *lb=new QLabel(this);
    QLabel *piclb=new QLabel(this);//窗体左上角图片Label

    piclb->setStyleSheet("QLabel{border-image: url(:/png/png/recording_32.png);}");
    piclb->setFixedSize(25,25);
    lb->setGeometry(25,0,28,20);
    lb->setText("存储为");//

    closeButton= new QToolButton(this);
    closeButton->setIcon(QIcon(":/png/png/close.png"));
    closeButton->setStyleSheet("background-color:transparent");
    connect(closeButton,&QToolButton::clicked,this,&Save::close);

    titleWid = new QWidget();
    titleLayout= new QHBoxLayout();
    titleLayout->addWidget(piclb);
    titleLayout->addWidget(lb);
    titleLayout->addWidget(closeButton);
    titleLayout->setSpacing(3);
    titleLayout->setMargin(0);
    titleWid->setFixedHeight(25);
    titleWid->setLayout(titleLayout);

    label1 = new QLabel("名称");
    label2 = new QLabel("位置");
    label3 = new QLabel("标签");
    QString Date=QDate::currentDate().toString(Qt::ISODate);

    lineEdit1 = new QLineEdit(Date);
    lineEdit2 = new LineEdit(this);
    desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    lineEdit2->setText(desktop_path);
    //    lineEdit2->setText("个人/录音");
    //    lineEdit2->installEventFilter(this);
    connect(lineEdit2, SIGNAL(clicked()), this, SLOT(savefile()));

    //    connect(lineEdit2,&QLineEdit::clicked,this,&save::select);connect(lineEdit2,&QLineEdit::clicked,this,&save::select);
    combobox = new QComboBox(this);
    QStringList str;//protected:
    //    virtual void mousePressEvent(QMouseEvent *event);
    str << "无" << "日记" << "行程"<< "心情";
    combobox->addItems(str);

    cancelButton= new QToolButton(this);
    cancelButton->setText("取消");
    connect(cancelButton,&QToolButton::clicked,this,&Save::close);

    saveButton= new QToolButton(this);
    saveButton->setText("保存");
    connect(saveButton,&QToolButton::clicked,this,&Save::saveToRecorder);//有点问题，需要修改

    mainWid = new QWidget(this);
    Wid = new QWidget();
    QGridLayout *pd=new QGridLayout();

//    class save
//    {
//    public:
//        save();
//    };
    pd->addWidget(label1,0,1,1,2);
    //    pg->setColumnStretch(0, 1);//protected:
    //    virtual void mousePressEvent(QMouseEvent *event);
    pd->addWidget(lineEdit1,0,2,1,4);
    pd->addWidget(label2,1,1,1,2);
    pd->addWidget(lineEdit2,1,2,1,4);
    pd->addWidget(label3,2,1,1,2);
    pd->addWidget(combobox,2,2,1,4);
    pd->addWidget(cancelButton,3,2,1,2);
    pd->addWidget(saveButton,3,4,1,2);
    Wid->setLayout(pd);


    mainLayout= new QVBoxLayout();
    mainLayout->addWidget(titleWid);
    mainLayout->addWidget(Wid);
    mainWid->setLayout( mainLayout);
    this->setCentralWidget(mainWid);
    this->setAttribute(Qt::WA_TranslucentBackground);//窗体透明
    this->move((QApplication::desktop()->width() -WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);
    mainWid->setObjectName("mainWid");//设置命名空间
    mainWid->setStyleSheet("#mainWid{border-radius:12px;background-color:#FFFFFF;border:1px solid #262626;}");//自定义窗体(圆角+背景色)


}
void Save::savefile()
{

//connect(this,SIGNAL(QLineEdit::clicked),MainWindow,SLOT(select()));
    MyThread *myth = new MyThread(this);

    if(saveData->get("type").toInt()==1)
    {
        myth->fileName = QFileDialog::getSaveFileName(
                          this,
                          tr("Select a file storage directory"),
                              QDir::currentPath(),
                              "Mp3(*.mp3)");
        myth->selectMp3();


    }
    else if(saveData->get("type").toInt()==3)
    {
        myth->fileName = QFileDialog::getSaveFileName(
                          this,
                          tr("Select a file storage directory"),
                              QDir::currentPath(),
                              "Wav(*.wav)");
        myth->selectWav();
    }
    else
    {

    }
    lineEdit2->setText(myth->fileName);


}
void Save::saveToRecorder()
{
    qDebug()<<lineEdit1->text();
    QString str=QFileDialog::getOpenFileName(
                this,
                tr("Select a file storage directory"),
                    QDir::currentPath(),
                    "Wav(*.wav)");
    lineEdit2->setText(str);

}

