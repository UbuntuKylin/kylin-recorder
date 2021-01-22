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
#ifndef CLIPBUTTON_H
#define CLIPBUTTON_H

#include <QWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QDebug>

class ClipButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ClipButton(QWidget *parent = nullptr);

    int posAx;
    int Arightx;
    int posBx;
    int Bleftx;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void pressPausePlayer_Signal();
    void leftBtn_ReleaseStartPlayer_Signal(int leftButton_absolutePos,int leftButton_rightBorderOppositive,int padding);
    void rightBtn_ReleaseGetEndPositon_Signal(int rightButton_absolutePos,int rightButton_leftBorderOppositive,int padding);

    void leftButton_rightBorderSignal(int x);//发送当前位置横坐标的信号
//    void rightButton_leftBorder(qint64 x);

private:
    QPoint pressPoint;

};

#endif // CLIPBUTTON_H
