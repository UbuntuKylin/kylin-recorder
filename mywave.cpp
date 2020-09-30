#include "mywave.h"

myWave::myWave(QWidget *parent) : QMainWindow(parent)
{
    this->initVariables();
}

void myWave::initVariables()
{
    m_nMin = 0;
    m_nMax = 0;
    m_nCurrentValue = 0;
    m_nValue = 0;
    m_nIncrement = 0;

    m_bReverse = false;

    m_updateTimer=new QTimer(this);
    m_updateTimer->setInterval(PYS_TIMER_INTERVAL);
    connect(m_updateTimer,SIGNAL(timeout()),this,SLOT(UpdateValue()));
    setMaximumWidth(PYS_WIDTH);
}

void myWave::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawSpectrum(&painter);
}

void myWave::drawSpectrum(QPainter *painter)//振幅、频率
{
    painter->save();
    painter->setPen(Qt::NoPen);

    qreal CenterY = (qreal) rect().center().y();


    qreal HalfHeight = (qreal) height()/2 - PYS_TOP_SPACE ;
    int Range = (m_nMax - m_nMin);//取值范围
    qreal YIncrement = (qreal)HalfHeight/Range;
    // draw top bar 画上半部分的频率图
    qreal TopRectY = YIncrement * m_nCurrentValue ;
    QPointF TopRectTopLeft(PYS_LEFT_SPACE, HalfHeight - TopRectY + PYS_TOP_SPACE );
    QPointF TopRectBottomRight(width()- PYS_LEFT_SPACE,CenterY);
    QRectF TopRect(TopRectTopLeft,TopRectBottomRight);

    QLinearGradient TopRectGradient(TopRect.topLeft(),TopRect.topRight());
    TopRectGradient.setColorAt(0.0,PYS_BAR_END_COLOR);
    TopRectGradient.setColorAt(0.5,PYS_BAR_START_COLOR);
    TopRectGradient.setColorAt(1.0,PYS_BAR_END_COLOR);
    painter->setBrush(TopRectGradient);

    painter->drawRect(TopRect);


    // draw bottom bar 画下半部分的频率图
    qreal BottomRectY = CenterY + YIncrement * m_nCurrentValue;
    QPointF BottomRectTopLeft(PYS_LEFT_SPACE,CenterY);
    QPointF BottomRectBottomRight(width() - PYS_LEFT_SPACE,BottomRectY);
    QRectF BottomRect(BottomRectTopLeft,BottomRectBottomRight);

    QLinearGradient BottomRectGradient(BottomRect.topLeft(),BottomRect.topRight());
    BottomRectGradient.setColorAt(0.0,PYS_BAR_END_COLOR);
    BottomRectGradient.setColorAt(0.5,PYS_BAR_START_COLOR);
    BottomRectGradient.setColorAt(1.0,PYS_BAR_END_COLOR);
    painter->setBrush(BottomRectGradient);
    painter->drawRect(BottomRect);
    painter->restore();
}

void myWave::setRange(int min, int max)
{
    m_nMin = min < max ? min : max;
    m_nMax = max > min ? max : min;
    m_nIncrement = ( m_nMax - m_nMin )/ PYS_INCREMENT_FACTOR;
}

void myWave::setMinimum(int min)
{
    m_nMin = min;
}

void myWave::setMaximum(int max)
{
    m_nMax = max;
}

void myWave::setValue(int value)
{
    if(m_nValue > value )
    {
        m_bReverse = true;
    }else if(m_nValue < value )
    {
        m_bReverse = false;
    }else{
        STOP_TIMER(m_updateTimer);
        return ;
    }

    m_nValue = value;
    STOP_TIMER(m_updateTimer);//
    m_updateTimer->start();
}

void myWave::UpdateValue()//更新波形图的矩形框大小
{
    if(m_bReverse)
    {
        m_nCurrentValue -= m_nIncrement;

        if(m_nCurrentValue < m_nValue)
        {
            m_nCurrentValue = m_nValue;
            STOP_TIMER(m_updateTimer);//
        }
    }else{
        m_nCurrentValue += m_nIncrement;

        if(m_nCurrentValue > m_nValue)
        {
            m_nCurrentValue = m_nValue;
            STOP_TIMER(m_updateTimer);//
        }
    }
    update();
}

int myWave::getValue()
{
    return m_nValue;
}
