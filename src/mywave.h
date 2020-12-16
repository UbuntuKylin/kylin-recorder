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
#ifndef MYWAVE_H
#define MYWAVE_H
#define PYS_TOP_SPACE 1
#define PYS_LEFT_SPACE 0
#define PYS_TIMER_INTERVAL 0//当矩形框数值变化时刷新频率
#define PYS_WIDTH 2
#define PYS_INCREMENT_FACTOR 100

#define PYS_BG_START_COLOR QColor(178,178,178)
#define PYS_BG_END_COLOR QColor(60,60,60)

#define PYS_BAR_START_COLOR QColor(229,78,78)
#define PYS_BAR_END_COLOR QColor(229,78,78)

#define PYS_APPLY_LINEAR_GRADIENT(Rect,StartPot,EndPot,StartColor,EndColor)  QLinearGradient Rect##Gradient(StartPot,EndPot); \

#include <QDebug>
#define STOP_TIMER(Timer) if(Timer->isActive()){ Timer->stop();}
#include <QMainWindow>
#include <QtGui>


class myWave : public QMainWindow
{
    Q_OBJECT
public:
    explicit myWave(QWidget *parent = nullptr);
public:
    void setRange(int min,int max);

    void setMinimum(int min);

    void setMaximum(int max);
public slots:
    void setValue(int value);
    int getValue();

protected:
    void paintEvent(QPaintEvent *);

    QSize sizeHint() const
    {
        return QSize(PYS_WIDTH,120);
    }

private:
//    void drawBackground(QPainter* painter);
    void drawSpectrum(QPainter* painter);

private:
    void initVariables();

private:
    int m_nMin;
    int m_nMax;
    int m_nCurrentValue;
    int m_nValue;
    int m_nIncrement;

    bool m_bReverse;

    QTimer* m_updateTimer;

private slots:
    void UpdateValue();


};

#endif // MYWAVE_H
