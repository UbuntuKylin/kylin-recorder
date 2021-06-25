#include "tools.h"
#include "mainwindow.h"

Tools::Tools(QObject *parent) : QObject(parent)
{

}

//获取文件存储的目录
QString Tools::getRecordingSaveDirectory(QString dirStr)
{
    QDir record;
    QLocale locale = QLocale::system().name();
    QDir recordDirectory;
    QString subDirectory = tr("Recorder");

    bool existEn = record.exists(dirStr+"Recorder");
    bool existCH = record.exists(dirStr+"录音");
//    qDebug()<<"目录分别是:"<<dirStr;
    recordDirectory = QDir(dirStr);
    if(!existEn&&!existCH)
    {
        /*
             * 判断中英文目录如果一个都没有则创建
             * English Environment:Recorder;
             * 中文环境:录音；
            */
        if(locale.language() == QLocale::English)
        {
//            qDebug()<<"English Environment:"<<dirStr+tr("Recorder");
            recordDirectory.mkdir(subDirectory);
        }
        else if(locale.language() == QLocale::Chinese)
        {
//            qDebug()<<"中文环境:"<<dirStr+tr("Recorder");
            recordDirectory.mkdir(subDirectory);
        }
        return recordDirectory.filePath(subDirectory);
    }
    else
    {
        /*
         * 至少有一个则不需要创建文件夹
         */
        if(existEn)
        {
            return recordDirectory.filePath("Recorder");
        }
        else if(existCH)
        {
            return recordDirectory.filePath("录音");
        }

    }
    return recordDirectory.filePath(subDirectory);
}

//文件信息列表
QFileInfoList Tools::getRecordingFileinfos()
{
    QStringList filters;
    filters << "*.wav"<<"*.mp3"<<"*.m4a";
    QStringList dirList = MainWindow::mutual->
            defaultPathData->get("recorderpath").toString().split(",");
//    qDebug()<<"目录集:"<<dirList<<"个数:"<<dirList.count();
    QFileInfoList fileInfoList ;
    for(int i = 1;i<dirList.count();i++)
    {
        QString dirStr = dirList.at(i);
//        qDebug()<<"目录"<<i<<dirStr;
        //将不同目录下的音频文件info统统加入fileInfoList
        fileInfoList << QDir(Tools::getRecordingSaveDirectory(dirStr)).entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    }
//    qDebug()<<"音频文件集:"<<fileInfoList;
    return fileInfoList;
}

//判断文件是否存在
bool Tools::fileExists(QString path)
{
    QFileInfo check_file(path);

    //检查文件是否存在，如果存在：它是否是一个文件而不是目录
    return check_file.exists() && check_file.isFile();
}

QString Tools::formatMillisecond(int millisecond)
{
    if (millisecond / 1000 < 3600) {
        //至少需要返回1秒。
        return QDateTime::fromTime_t(std::max(1, millisecond / 1000)).toUTC().toString("hh:mm:ss");
    } else {
        return QDateTime::fromTime_t(millisecond / 1000).toUTC().toString("hh:mm:ss");
    }
}
