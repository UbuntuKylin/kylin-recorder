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
#include "clipbutton.h"
#include "itemswindow.h"

ClipButton::ClipButton(QWidget *parent) : QToolButton(parent)
{
}
void ClipButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        this->raise(); //将此按钮移动到顶层显示
        this->pressPoint = event->pos();
        if(this->objectName() == "leftBtn")
        {
            emit pressPausePlayer_Signal();//发送音频暂停的信号
        }
    }
}

void ClipButton::mouseReleaseEvent(QMouseEvent *event)
{
    int leftButton_absolutePos = 0;
    int rightButton_absolutePos = 0;
    QList <QObject *>list = this->parent()->children();
    int padding ;//滑块内距
    if(event->button() == Qt::LeftButton){

        if(this->objectName() == "leftBtn")
        {
            leftButton_absolutePos = this->mapToParent(event->pos() - this->pressPoint).rx()+this->width();
            rightButton_absolutePos = qobject_cast<QToolButton *> (list.at(list.count()-2))->pos().rx();
            padding = rightButton_absolutePos - (this->pos().rx()+this->width());
            qDebug()<<"左滑块的右边界的位置:"<<leftButton_absolutePos<<"两滑块内距离:"<<padding;
            //发送左滑块右边界的相对位置的信号
            emit leftBtn_ReleaseStartPlayer_Signal(leftButton_absolutePos,this->pos().rx(),padding);
        }
        else if(this->objectName() == "rightBtn")
        {
            rightButton_absolutePos = this->mapToParent(event->pos() - this->pressPoint).rx();
            leftButton_absolutePos = qobject_cast<QToolButton *> (list.at(list.count()-2))->pos().rx()+this->width();
            padding = this->pos().rx() - leftButton_absolutePos;
            qDebug()<<"右滑块的左边界的位置:"<<rightButton_absolutePos<<"两滑块内距离:"<<padding;
            qDebug()<<"当前按钮的位置:"<<this->pos().rx();
            emit rightBtn_ReleaseGetEndPositon_Signal(rightButton_absolutePos,this->pos().rx() - this->width(),padding);
        }
        else
        {
            qDebug()<<"游标";

        }

    }

}

void ClipButton::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton){
        //qDebug()<<event->pos()<<this->pressPoint<<this->mapToParent(event->pos() - this->pressPoint);
        //qDebug()<<this->objectName();
        if(this->objectName() == "leftBtn")
        {
            //qDebug()<<this->pos().rx();
            posAx = this->mapToParent(event->pos() - this->pressPoint).rx();
            Arightx = posAx+this->width();
            QList <QObject *>list = this->parent()->children();
            Bleftx = qobject_cast<QToolButton *> (list.at(list.count()-2))->pos().rx();
//            qDebug()<<"左箭头的位置:"<<posAx<<"左箭头的右侧:"<<Arightx<<";"<<"右箭头的左侧:"<<Bleftx;
//            emit leftButton_rightBorderSignal(posAx);
            if(Arightx >= Bleftx-5 )
            {
                posAx = Bleftx-5-this->width();
            }
            if(this->pos().rx() == posAx)
            {
                return;
            }
            this->move(posAx,this->pos().ry());
        }
        else if(this->objectName()=="rightBtn")
        {
            QList <QObject *>list = this->parent()->children();
            Arightx = qobject_cast<QToolButton *> (list.at(list.count()-2))->pos().rx()+qobject_cast<QToolButton *> (list.at(list.count()-2))->width();
            posBx = this->mapToParent(event->pos() - this->pressPoint).rx();
//            qDebug()<<"右箭头的左侧:"<<posBx<<";"<<"左箭头的右侧:"<<Arightx;
            if(posBx<=Arightx+5)
            {
                posBx = Arightx+5;
            }
            if(this->pos().rx() == posBx)
            {
               return;
            }
            this->move(posBx,this->pos().ry());
        }
        //防止按钮移出父窗口
        if(this->mapToParent(this->rect().topLeft()).x() <= 0){
            //qDebug()<<this->pos().y();
            this->move(0, this->pos().y());
        }
        if(this->mapToParent(this->rect().bottomRight()).x() >= this->parentWidget()->rect().width()){
            this->move(this->parentWidget()->rect().width() - this->width(), this->pos().y());
        }
        if(this->mapToParent(this->rect().topLeft()).y() <= 0){
            this->move(this->pos().x(), 0);
        }
        if(this->mapToParent(this->rect().bottomRight()).y() >= this->parentWidget()->rect().height()){
            this->move(this->pos().x(), this->parentWidget()->rect().height() - this->height());
        }
    }
}

