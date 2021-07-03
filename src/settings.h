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
#ifndef SETTINGS_H
#define SETTINGS_H

#include "save.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QRadioButton>
#include <QLineEdit>
#include <QButtonGroup>
#include <QGridLayout>
#include <QSettings>
#include <QGSettings>
#include <QRect>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

#include "daemondbus.h"
#include "lineedit.h"
#include "mylabel.h"

class Settings : public QMainWindow
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    QGSettings  *Data= nullptr;
    QGSettings  *darkData= nullptr;
    QString defaultLocation;

    QWidget *mainWid;
    QToolButton *closeButton;
    // 用户手册功能
    DaemonDbus *mDaemonIpcDbus;
    // 键盘响应事件
    void keyPressEvent(QKeyEvent *event);

private:

    QButtonGroup *buttonGroup;

    QHBoxLayout *titleLayout;
    QGridLayout *pg;

    QVBoxLayout *mainLayout;

    QWidget *titleWid;

    QWidget *centerWid;//中心wid4行
    QWidget *fileStoreWid;//文件存储wid
    QWidget *fileFormatWid;//文件格式wid
    QWidget *sourceWid;//录音来源wid
    QWidget *btnWid;//按钮wid
    QVBoxLayout *centerLayout;
    QHBoxLayout *fileStoreLayout;//文件存储Layout
    QHBoxLayout *fileFormatLayout;//文件格式Layout
    QHBoxLayout *sourceLayout;//录音来源Layout
    QHBoxLayout *btnLayout;//按钮Layout


    QLabel *storelabel;
//    LineEdit *storeEdit;//文件存储//重写的Linedit因为QLineEdit没有点击事件
    MyLabel *pathLabel;//路径标签
    QPushButton *alterBtn;//更改btn
    QLabel *formatLabel;//文件格式
    QComboBox *formatDownList;//格式下拉列表mp3、wav、m4a
    QLabel *sourceLabel;//来源标签
    QComboBox *sourceDownList;//录音来源下拉列表mp3、wav、m4a
    QPushButton *OKBtn;//确认按钮

    QString fileName;
    QString selectDirPath;

    int inputEditJudge(QString fileDir);//文本框编辑或编程使其改变时要判断非法字符
public slots:

    void openFileDialog();//点击文本框时触发

    void editSlot();//文本框编辑时触发
    void saveType(int index);//选择时保存的类型
    void recordSource(int index);//选中mic录音源

//    void closesave();
//    void gotosave();
//    void saveTypeMp3();
//    void saveTypeM4a();
//    void saveTypeWav();
//    void allAudio();
//    void inside();
//    void microphone();
signals:

};

#endif // SETTINGS_H
