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
<<<<<<< HEAD:src/settings.h
 *  Authors: baijincheng <baijincheng@kylinos.cn>
=======
 * Authors: baijincheng <baijincheng@kylinos.cn>
>>>>>>> b83fa1d75587ad7c1b69c42f9dfc7e3e3e6e80ca:settings.h
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

#include "daemondbus.h"

class Settings : public QMainWindow
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    QGSettings  *Data= nullptr;
    QGSettings  *darkData= nullptr;
    //int save_default = 1;
    QRadioButton *radioButton;
    //    QLabel *label_2;
    QRadioButton *radioButton_2;
    QString defaultLocation;

    QRadioButton *radioButton_3;
    QLabel *label_5;

    QRadioButton *radioButton_6;
    QLabel *label_8;


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

    QWidget *Wid;

    QLabel *label;
    //    QRadioButton *radioButton;
    QLabel *label_2;
    //    QRadioButton *radioButton_2;
    QLabel *label_3;
    QLabel *label_3_0;
    QLabel *label_3_1;
    QLabel *label_3_2;
    QLineEdit *lineEdit;
    QLabel *label_4;


    QRadioButton *radioButton_4;
    QLabel *label_6;
    QRadioButton *radioButton_5;
    QLabel *label_7;

    QRadioButton *radioButton_7;
    QLabel *label_9;
    QRadioButton *radioButton_8;
    QLabel *label_10;
    QLabel *label_11;
public slots:
    void closesave();
    void gotosave();
    void saveTypeMp3();
    void saveTypeM4a();
    void saveTypeWav();
    void allAudio();
    void inside();
    void microphone();
signals:

};

#endif // SETTINGS_H
