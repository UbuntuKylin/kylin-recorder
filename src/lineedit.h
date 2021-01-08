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
#ifndef LINEEDIT_H
#define LINEEDIT_H
#include <QMouseEvent>
#include <QLineEdit>
class LineEdit: public QLineEdit
{
    Q_OBJECT
public:
   LineEdit();

    explicit LineEdit(QWidget *parent = 0);
protected:
    //重写mousePressEvent事件
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    //自定义clicked()信号,在mousePressEvent事件发生时触发
    void clicked();

public slots:
};

#endif // MYLINEEDIT_H
