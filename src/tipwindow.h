#ifndef TIPWINDOW_H
#define TIPWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDialog>

class TipWindow : public QDialog
{
    Q_OBJECT
public:
    explicit TipWindow(QWidget *parent = nullptr);
    QWidget *mainWid;
    QLabel *tipLabel;
    QHBoxLayout *hLayout;
    void setTipWindow();

signals:

};

#endif // TIPWINDOW_H
