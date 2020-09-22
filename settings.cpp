#include "settings.h"
#include "mainwindow.h"
#include <QLabel>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QtCore/QCoreApplication>
Settings::Settings(QWidget *parent) : QMainWindow(parent)
{
    //setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
        int WIDTH=360;
        int HEIGHT=300;
        this->resize(WIDTH,HEIGHT);
        setWindowFlags(Qt::FramelessWindowHint);

    //标题栏设置和布局
        QLabel *lb=new QLabel(this);
        QLabel *piclb=new QLabel(this);//窗体左上角图片Label

        piclb->setStyleSheet("QLabel{border-image: url(:/png/png/recording_32.png);}");
        piclb->setFixedSize(25,25);
        lb->setText("设置");//

        closeButton= new QToolButton(this);
        closeButton->setIcon(QIcon(":/png/png/close.png"));
        closeButton->setStyleSheet("background-color:transparent");
        closeButton->setFixedSize(30,30);
        closeButton->setStyleSheet("QToolButton{border-radius:4px;}"
                                   "QToolButton:hover{background-color:#F04234}"
                                   "QToolButton:pressed{background-color:#D83436}");
        connect(closeButton,&QToolButton::clicked,this,&Settings::close);


        titleWid = new QWidget(this);
        titleLayout= new QHBoxLayout(this);
        titleLayout->addWidget(piclb);
        titleLayout->addWidget(lb);
        titleLayout->addWidget(closeButton);
        titleLayout->setSpacing(1);
        titleLayout->setMargin(0);
        titleWid->setFixedHeight(30);
        titleWid->setLayout(titleLayout);

    //网格布局设置界面主体内容
    //        QGridLayout *layout = new QGridLayout();
    //设置界面内容
        label = new QLabel("文本存储");
        radioButton = new QRadioButton(this);
    //    radioButton->setCheckable(1);
    //    radioButton->setChecked(1);//默认选中状态

        connect(radioButton,&QRadioButton::clicked,this,&Settings::gotosave);

        label_2 = new QLabel("存储为");
        radioButton_2 = new QRadioButton(this);

    //    connect(radioButton_2,&QRadioButton::clicked,this,&subMainWindow::userdirectory);
        connect(radioButton_2,&QRadioButton::clicked,this,&Settings::closesave);
        qDebug()<<"3333333";

        label_3 = new QLabel("默认存储：");
        lineEdit = new QLineEdit(this);
        desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        lineEdit->setText(desktop_path);
        lineEdit->setFocusPolicy(Qt::NoFocus);//设置不可编辑状态
        label_4 = new QLabel("录音来源");
        radioButton_3 = new QRadioButton(this);
        label_5 = new QLabel("全部");
        radioButton_4 = new QRadioButton();
        label_6 = new QLabel("系统内部");
        radioButton_5 = new QRadioButton(this);
        label_7 = new QLabel("麦克风");
        label_8 = new QLabel("文件格式");
        radioButton_6 = new QRadioButton(this);
        label_9 = new QLabel("mp3");
        radioButton_7 = new QRadioButton();
        label_10 = new QLabel("m4a");
        radioButton_8 = new QRadioButton(this);
        label_11 = new QLabel("wav");

    //按钮分组，网格布局
        QButtonGroup *grp1=new QButtonGroup(this);
        grp1->addButton(radioButton);
        grp1->addButton(radioButton_2);
        QButtonGroup *grp2=new QButtonGroup(this);
        grp2->addButton(radioButton_3);
        grp2->addButton(radioButton_4);
        grp2->addButton(radioButton_5);
        QButtonGroup *grp3=new QButtonGroup(this);
        grp3->addButton(radioButton_6);
        grp3->addButton(radioButton_7);
        grp3->addButton(radioButton_8);

    //布局
        mainWid = new QWidget(this);
        Wid = new QWidget();
        QGridLayout *pg=new QGridLayout();
    //    pg->setVerticalSpacing(22);
        pg->addWidget(label,0,0,1,1);
    //    pg->setColumnStretch(0, 1);
        pg->addWidget(radioButton,0,2,1,1);
        pg->addWidget(label_2,0,3,1,1);
        pg->addWidget(radioButton_2,1,2,1,1);
        pg->addWidget(label_3,1,3,1,1);
        pg->addWidget(lineEdit,2,3,1,6);
        pg->addWidget(label_4,3,0,1,2);
        pg->addWidget(radioButton_3,3,2,1,1);
        pg->addWidget(label_5,3,3,1,1);
        pg->addWidget(radioButton_4,3,4,1,1);
        pg->addWidget(label_6,3,5,1,1);
        pg->addWidget(radioButton_5,3,6,1,1);
        pg->addWidget(label_7,3,7,1,1);
        pg->addWidget(label_8,4,0,1,2);
        pg->addWidget(radioButton_6,4,2,1,1);
        pg->addWidget(label_9,4,3,1,1);
        pg->addWidget(radioButton_7,4,4,1,1);
        pg->addWidget(label_10,4,5,1,1);
        pg->addWidget(radioButton_8,4,6,1,1);
        pg->addWidget(label_11,4,7,1,1);
        Wid->setLayout(pg);

        mainLayout= new QVBoxLayout(this);
        mainLayout->addWidget(titleWid);
        mainLayout->addWidget(Wid);
        mainWid->setLayout( mainLayout);
        this->setCentralWidget(mainWid);
        this->move((QApplication::desktop()->width() -WIDTH)/3, (QApplication::desktop()->height() - HEIGHT)/3);
        this->setAttribute(Qt::WA_TranslucentBackground);//窗体透明
        mainWid->setStyleSheet("border-radius:12px;background-color:#FFFFFF;");//自定义窗体(圆角+背景色)
        //    QSettings  *my_ini= new QSettings(, QSettings::IniFormat);
        QSettings my_ini;//******************************存储为/默认为
        if(my_ini.value("save_default") == 1){

              radioButton->setChecked(1);
             }
    else{

             radioButton_2->setChecked(1);

         }
}
void Settings::gotosave(){

    my_ini.setValue("save_default",1);

}
void Settings::closesave(){
    my_ini.setValue("save_default",0);

}
