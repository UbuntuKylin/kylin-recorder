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
    int WIDTH=360;
    int HEIGHT=300;
    this->resize(WIDTH,HEIGHT);
    setWindowFlags(Qt::FramelessWindowHint);

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

    titleWid = new QWidget(this);
    titleLayout= new QHBoxLayout(this);
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

    lineEdit1 = new QLineEdit("2020.8.20");
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

    connect(saveButton,&QToolButton::clicked,this,&Save::close);//有点问题，需要修改
    mainWid = new QWidget(this);
    Wid = new QWidget();
    QGridLayout *pd=new QGridLayout();

    class save
    {
    public:
        save();
    };
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


    mainLayout= new QVBoxLayout(this);
    mainLayout->addWidget(titleWid);
    mainLayout->addWidget(Wid);
    mainWid->setLayout( mainLayout);
    this->setCentralWidget(mainWid);
    this->move((QApplication::desktop()->width() -WIDTH)/2, (QApplication::desktop()->height() - HEIGHT)/2);
}
void Save::savefile()
{

//connect(this,SIGNAL(QLineEdit::clicked),MainWindow,SLOT(select()));
    MyThread *diaoyong = new MyThread(this);
    diaoyong->select();
    lineEdit2->setText(diaoyong->fileName);

}
