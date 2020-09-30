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
