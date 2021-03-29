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
 * Authors: baijincheng <baijincheng@kylinos.cn>
 */
#include "mainwindow.h"
#include <QTranslator>
#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <fcntl.h>
#include <syslog.h>

#define DOUBLE 2
#define MAX_FILE_SIZE 1024
#define LOG_FILE0 "demo_0.log"
#define LOG_FILE1 "demo_1.log"
#define LOG_FILE_PATH "/.cache/kylin-recorder/log"
/************************************************
* 函数名称：qtMessagePutIntoLog
* 功能描述：日志记录函数
* 输入参数：qDebug标准输出
* 输出参数：kylinRecorderLog.txt
* 修改日期：2021.03.22
* 修改内容：
*   创建  bjc
*
*************************************************/
void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    QString name[DOUBLE] = {LOG_FILE0, LOG_FILE1};
    FILE *log_file = nullptr;
    QString logFilePath;
    int fileSize;
    static int i = 0;
    QDir dir;
    bool flag = false;

    logFilePath = QDir::homePath() + LOG_FILE_PATH;
    if (dir.mkpath(logFilePath)) {
        flag = true;
    }

    if (flag) {
        logFilePath = logFilePath + "/" + name[i];
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file) {
        fileSize = ftell(log_file);
        if (fileSize >= MAX_FILE_SIZE) {
            i = (i + 1) % DOUBLE;
            logFilePath = QDir::homePath() + LOG_FILE_PATH + "/" + name[i];
            if (QFile::exists(logFilePath)) {
                QFile temp(logFilePath);
                temp.remove();
            }
        }
        fclose(log_file);
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageOutput);//日志输出功能
    /*
     * 添加4K 屏幕支持。
     */
    #if(QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #else
        // do not support 4K.
    #endif
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
      QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
      QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
      QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #endif
    QApplication a(argc, argv);


    QTranslator app_trans;
    QTranslator qt_trans;
    QString locale = QLocale::system().name();

    QString trans_path;
    if (QDir("/usr/share/kylin-recorder/translations").exists()) {
        trans_path = "/usr/share/kylin-recorder/translations";
    }
    else {
        trans_path = qApp->applicationDirPath() + "/translations";
    }
    QString qt_trans_path;
    qt_trans_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);// /usr/share/qt5/translations

    if (locale == "zh_CN") {
        if(!app_trans.load("kylin-recorder_" + locale + ".qm", trans_path))
            qDebug() << "Load translation file："<< "kylin-recorder_" + locale + ".qm from" << trans_path << "failed!";
        else
            a.installTranslator(&app_trans);

        if(!qt_trans.load("qt_" + locale + ".qm", qt_trans_path))
            qDebug() << "Load translation file："<< "qt_" + locale + ".qm from" << qt_trans_path << "failed!";
        else
            a.installTranslator(&qt_trans);
    }
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    QTranslator *translator = new QTranslator();
    if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        a.installTranslator(translator);
    }
    else
    {
        qDebug() << "加载中文失败";
    }
    MainWindow w;

    // 添加窗管协议
//        MotifWmHints hints;
//        hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
//        hints.functions = MWM_FUNC_ALL;
//        hints.decorations = MWM_DECOR_BORDER;
//        XAtomHelper::getInstance()->setWindowMotifHint(w.winId(), hints);

//    w.show();

    return a.exec();
}
