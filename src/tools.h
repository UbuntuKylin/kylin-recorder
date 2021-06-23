#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QFileInfoList>
#include <QDateTime>

class Tools : public QObject
{
    Q_OBJECT
public:
    explicit Tools(QObject *parent = nullptr);
    static QString getRecordingSaveDirectory(QString dirStr);
    static bool fileExists(QString path);
    static QFileInfoList getRecordingFileinfos();
    static QString formatMillisecond(int millisecond);


};

#endif // TOOLS_H
