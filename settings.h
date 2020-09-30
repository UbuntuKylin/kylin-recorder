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

class Settings : public QMainWindow
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    QSettings my_ini;
    int save_default = 1;
    QRadioButton *radioButton;
    //    QLabel *label_2;
    QRadioButton *radioButton_2;
    QString desktop_path;
private:
    QToolButton *closeButton;
    QButtonGroup *buttonGroup;

    QHBoxLayout *titleLayout;
    QGridLayout *pg;

    QVBoxLayout *mainLayout;

    QWidget *titleWid;
    QWidget *mainWid;
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
    QRadioButton *radioButton_3;
    QLabel *label_5;
    QRadioButton *radioButton_4;
    QLabel *label_6;
    QRadioButton *radioButton_5;
    QLabel *label_7;
    QRadioButton *radioButton_6;
    QLabel *label_8;
    QRadioButton *radioButton_7;
    QLabel *label_9;
    QRadioButton *radioButton_8;
    QLabel *label_10;
    QLabel *label_11;
public slots:
    void closesave();
    void gotosave();
signals:

};

#endif // SETTINGS_H
