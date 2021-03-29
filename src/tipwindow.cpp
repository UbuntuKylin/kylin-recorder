#include "tipwindow.h"
#include "mainwindow.h"

#include "xatom-helper.h"

TipWindow::TipWindow(QWidget *parent) : QDialog(parent)
{
    int WIDTH=300;
    int HEIGHT=50;
//    mainWid = new QWidget();
    tipLabel = new QLabel();
    hLayout = new QHBoxLayout();
    this->setFixedSize(WIDTH,HEIGHT);
    setTipWindow();


}

void TipWindow::setTipWindow()
{
    tipLabel->setText(tr("Transcoding..."));
    tipLabel->setFixedSize(150,30);
    this->setWindowFlag(Qt::Tool);//此代码必须在此窗管协议前，否则此模态窗口背景不变灰
    hLayout->addWidget(tipLabel);
    this->setLayout(hLayout);
    // 添加窗管协议
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);
    this->setAttribute(Qt::WA_ShowModal, true);//模态窗口
}

