#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    /*
     * 添加4K 屏幕支持。
     */
#if(QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#else
    // do not support 4K.
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
