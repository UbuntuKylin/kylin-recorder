#ifndef SAVE_H
#define SAVE_H

#include <QMainWindow>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>

class Save : public QMainWindow
{
    Q_OBJECT
public:
    explicit Save(QWidget *parent = nullptr);

    QGridLayout *pd;
    QLabel *label;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;

    QLineEdit *lineEdit1;
    QLineEdit *lineEdit2;

    QComboBox *combobox;

    QToolButton *closeButton;
    QToolButton *cancelButton;
    QToolButton *saveButton;

    QHBoxLayout *titleLayout;
    QVBoxLayout *mainLayout;

    QWidget *titleWid;
    QWidget *mainWid;
    QWidget *Wid;
    QString desktop_path;

signals:
    void clicked();
public slots:
    void savefile();


};

#endif // SAVE_H
