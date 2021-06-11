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
#include "mythread.h"
#include "mainwindow.h"

struct WAVFILEHEADER
{
    char RiffName[4];// RIFF 头(标志)
    uint32_t RiffLength;//从下一个开始到文件结尾的字节数
    char WavName[4];// WAVE 文件
    char FmtName[4];// fmt 波形格式

    uint32_t FmtLength;// 数据格式
    uint16_t AudioFormat;//音频格式是否是压缩的PCM编码，1无压缩，大于1有压缩
    uint16_t ChannleNumber;//声道数量
    uint32_t SampleRate;//采样频率
    uint32_t BytesPerSecond;// byterate(每秒字节数) = 采样率(Hz)*样本数据位数(bit)*声道数/8
    uint16_t BytesPerSample;//块对齐/帧大小 framesize = 通道数 * 数据位数
    uint16_t BitsPerSample; //样本数据位数

    uint32_t DataLength;//实际音频数据的大小
    char DATANAME[4];   //数据块
};

MyThread::MyThread(QWidget *parent) : QWidget(parent)
{

    qDebug()<<"子线程MyThread::startThreadSlot QThread::currentThreadId()=="<<QThread::currentThreadId();

    audioInputSound = nullptr;
    audioOutputSound = nullptr;
    inputDevSound = nullptr;
    audioRecorder = new QAudioRecorder;
    recordData = new QGSettings(KYLINRECORDER);

    my_time=new QTimer;//延时检测
    tipTimer = new QTimer;//延时process,防止tip界面卡顿//不需要了
    audioRecorder = new QAudioRecorder;
    audioProbe = new QAudioProbe;
    connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));
    audioProbe->setSource(audioRecorder);




    process = new QProcess;//调用外部程序如：ffmpeg进行音频文件转码

    connect(process,SIGNAL(started()),this,SLOT(uploadStarted()));
//    connect(process, SIGNAL(finished(int)), this, SLOT(audioConversionFinish(int)), Qt::UniqueConnection);
    connect(process,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(audioConversionFinish(int, QProcess::ExitStatus)));
    connect(this,&MyThread::handling,MainWindow::mutual,&MainWindow::handlingSlot);//处理中...

    connect(my_time,&QTimer::timeout,this,[=]{
        canMonitor=true;
    });
    connect(this,&MyThread::listItemAddSignal,MainWindow::mutual,&MainWindow::slotListItemAdd);

}

//pcm转换wav
qint64 MyThread::toConvertWAV(QString catheFileName , QString filename)
{
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");
    qstrcpy(WavFileHeader.DATANAME, "data");

    WavFileHeader.FmtLength = 16;  //  表示 FMT 的长度
    WavFileHeader.AudioFormat = 1; //这个表示 PCM 编码无压缩;
    WavFileHeader.BitsPerSample = 16;//采样位数
    WavFileHeader.BytesPerSample = 2;
    WavFileHeader.SampleRate = 48000;//采样频率
    WavFileHeader.BytesPerSecond = 96000;//byterate(每秒字节数)=采样率(Hz)*采样位数(bit)*声道数/8
    WavFileHeader.ChannleNumber = 1;//音频通道数平声道

//    endFileName = isSameFileName(filename);//判断是否文件名重复
//    qDebug()<<"endFileName = "<<endFileName;
    QFile cacheFile(catheFileName);
    QFile wavFile(endFileName);
    if (!cacheFile.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    if (!wavFile.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    int nSize = sizeof(WavFileHeader);
    qint64 nFileLen = cacheFile.bytesAvailable();
    // 先将wav文件头信息写入，再将音频数据写入;
//    wavFile.write((char *)&WavFileHeader, nSize);//wav特有的写入方式2021.4.9暂时禁掉
//    wavFile.write(cacheFile.readAll());//wav特有的写入方式2021.4.9暂时禁掉
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(endFileName).data();
    QString cmd="ffmpeg -y -threads 2 -f s16le -ar 48k -ac 1 -i \""+catheFileName+"\" \""+endFileName+"\"";
    process->start(cmd);//0609暂时去掉后期复原

    cacheFile.close();
    wavFile.close();
    return nFileLen;
}

//pcm转换mp3
qint64 MyThread::toConvertMp3(QString catheFileName , QString mp3FileName)
{

    QFile cacheFile(catheFileName);
    QFile mp3File(endFileName);
    if (!cacheFile.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    if (!mp3File.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    qint64 nFileLen = cacheFile.bytesAvailable();
    QByteArray ba = cacheFile.readAll();

    qDebug() <<"缓存长度:"<<ba.size()<< nFileLen ;
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(endFileName).data();
    QString cmd="ffmpeg -y -threads 2 -f s16le -ar 48k -ac 2 -i \""+catheFileName+"\" \""+endFileName+"\"";
    process->start(cmd);//0609暂时去掉后期复原
    cacheFile.close();
    mp3File.close();

    return nFileLen;
}

qint64 MyThread::toConvertM4a(QString catheFileName , QString m4aFileName)
{
//    endFileName = isSameFileName(m4aFileName);//判断是否文件名重复
    QFile cacheFile(catheFileName);
    QFile m4aFile(endFileName);
    if (!cacheFile.open(QIODevice::ReadWrite))
    {
        return -1;
    }
    if (!m4aFile.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    qint64 nFileLen = cacheFile.bytesAvailable();
    QByteArray ba = cacheFile.readAll();

    qDebug() <<ba.size()<< nFileLen ;
    //ffmpeg -y -f s16le -ar 16k -ac 1 -i input.raw output.mp3//ffmpeg真香
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
   // code->fromUnicode(catheFileName).data();
    code->fromUnicode(endFileName).data();
    QString cmd="ffmpeg -y -f s16le -ar 48k -ac 2 -i \""+catheFileName+"\" \""+endFileName+"\"";
    //qDebug()<<"******"<<catheFileName<<"*"<<cmd<<"******";

    process->start(cmd);//0609暂时去掉后期复原


    cacheFile.close();
    m4aFile.close();

    return nFileLen;
}

void MyThread::uploadStarted()
{
    qDebug()<<"执行中!!!";
    emit handling(true);
}

void MyThread::audioConversionFinish(int isOk,QProcess::ExitStatus)
{
    if(isOk==0)
    {
        qDebug() << "音频格式转换成功"<<isOk;
        isSuccess=isOk;
//        MainWindow::mutual->WrrMsg->hide();
        MainWindow::mutual->tipWindow->hide();

    }
    else
    {
        qDebug() << "失败"<<isOk;
        isSuccess=isOk;
        MainWindow::mutual->tipWindow->hide();
    }


}

QString MyThread::pathSetting()
{
    QString outputName;//输出的文件路径名
    QString pathStr = recordData->get("path").toString();
    QTime t1;
    t1=QTime::currentTime();
    QString timeStr = t1.toString("hh:mm:ss");
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    default_Location = setDefaultPath(defaultPath);//设置默认存储到录音的路径
    QDateTime current_time = QDateTime::currentDateTime();
    //显示时间，格式为：年-月-日 时：分：秒 周几,可自行定义QDateTime::currentDateTime().toString("yyyyMMddhhmmss")
    QString StrCurrentTime = current_time.toString("yyyy.MM.dd");
    fileName = StrCurrentTime;

    if(recordData->get("path").toString() == ""||pathStr.split("/").last() == "")
    {
        qDebug()<<"配置文件中路径:"<<recordData->get("path").toString();
        qDebug()<<"最后一个'/'后面的字符串是"<<pathStr.split("/").last();
        if(recordData->get("type").toInt()==1)//mp3
        {
            outputName = default_Location+tr("/")+fileName+tr("-")+timeStr+tr(".mp3");
            format=Mp3();
        }
        else if(recordData->get("type").toInt()==3)//wav
        {
            outputName = default_Location+tr("/")+fileName+tr("-")+timeStr+tr(".wav");
            format=Wav();
        }
        else
        {
            outputName = default_Location+tr("/")+fileName+tr("-")+timeStr+tr(".m4a");
            format=M4a();
        }
    }
    else
    {
        QString tempStr = recordData->get("path").toString();
        qDebug()<<"尾部'/'后有字符串的路径:"<<tempStr;
        if(recordData->get("type").toInt()==1)//mp3
        {
            outputName = tempStr+tr("/")+fileName+tr("-")+timeStr+tr(".mp3");
            format=Mp3();
        }
        else if(recordData->get("type").toInt()==3)//wav
        {
            outputName = tempStr+tr("/")+fileName+tr("-")+timeStr+tr(".wav");
            format=Wav();
        }
        else
        {
            outputName = tempStr+tr("/")+fileName+tr("-")+timeStr+tr(".m4a");
            format=M4a();
        }
    }
    return outputName;
}

//按压录音按钮之后触发
void MyThread::record_pressed()
{
    qDebug()<<"开始录音:";
    //开始前，count先从0开始
    count=0;
    my_time->start(150);//每隔xms检测一次，波形图缓慢刷新，这行代码一定在前面
    outputFileName = pathSetting();//根据配置文件更新录音文件的路径
    audioInputFile = new QAudioInput(format, this);
    file =new QFile();
    QString str = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    absolutionPath = str + "/.cache/record.raw";//raw缓存文件放在绝对路径
    qDebug()<<"绝对路径:"<<absolutionPath;
    file->setFileName(absolutionPath);
    bool is_open =file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(!is_open)
    {
        qDebug()<<"打开失败";
        exit(1);
    }
    endFileName = isSameFileName(outputFileName);//判断是否文件名重复
    onChangeCurrentRecordList(endFileName);//更新路径配置文件
    InitMonitor();
    qDebug()<<"录音开始";
    audioInputFile->start(file);
    if(!audioRecorder->setOutputLocation(QUrl::fromLocalFile(endFileName))){

        qDebug() << "文件保存有毒！退出！";
        return ;
    }
    QStringList audioCodecsList = audioRecorder->supportedAudioCodecs();
    QAudioEncoderSettings settings;
    settings.setCodec(audioCodecsList[0]);
    settings.setSampleRate(16000);  //采样率   XXXXXXX
    settings.setQuality(QMultimedia::EncodingQuality(10));
    settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
    audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings());
    audioRecorder->record();

}



//Mp3格式
QAudioFormat MyThread::Mp3()
{
    QAudioFormat format = QAudioFormat();
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，48000HZ(标准CD级)，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(2);//立体声，数目为2
    format.setSampleSize(16);
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//低位优先
    format.setSampleType(QAudioFormat::SignedInt);//QAudioFormat::SignedInt

    return format;
}
//Wav格式
QAudioFormat MyThread::Wav()
{
    QAudioFormat format = QAudioFormat();//设置采样格式
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(1);//设置通道数：平声道，数目为1
    format.setSampleSize(16);//设置采样大小，一般为8位或16位
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//设置字节序，低位优先
    format.setSampleType(QAudioFormat::SignedInt);//设置样本数据类型
    return format;
}

//格式M4a格式
QAudioFormat MyThread::M4a()
{
    QAudioFormat format = QAudioFormat();
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(2);//立体声，数目为2
    format.setSampleSize(16);
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//低位优先
    format.setSampleType(QAudioFormat::SignedInt);//v10.1规定QAudioFormat::SignedInt。
    return format;
}

void MyThread::saveAs(QString oldFileName)//右键另存为可以选择存储音频格式
{
    QFileDialog *fileDialog = new QFileDialog;
    QString fileType = "Mp3(*.mp3);;Wav(*.wav);;M4a(*.m4a)";
    fileDialog->setNameFilter(fileType);
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    qDebug()<<defaultPath;
    //QFileDialog指定父窗口,使系统弹窗在中间MainWindow::mutual->mainWid
    QString newFileName = QFileDialog::getSaveFileName(MainWindow::mutual->mainWid,
                                                     tr("Select a file storage directory"),
                                                     defaultPath+"/RecordFile.mp3",
                                                     fileType);
    QMessageBox *mBox = new QMessageBox(this);
    QMessageBox::StandardButton ret = mBox->standardButton(mBox->clickedButton());
    if(ret == QMessageBox::Ok && newFileName ==""||newFileName.contains(" ")||newFileName.contains("?")||newFileName.contains("'")||newFileName.contains("\"")||newFileName.contains("\\"))
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,
                             tr("Warning"),tr("Do not enter illegal file name"),QMessageBox::Ok);
        qDebug()<<"进来了!!";
        return ;
    }
    if(oldFileName != newFileName)
    {
        if(newFileName.split(".").last() == "mp3")
        {
            qDebug() <<"旧文件名:"<<oldFileName<<"新文件名:"<<newFileName<<newFileName.split(".").last();
            QString cmd="ffmpeg -y -i \""+oldFileName+"\" \""+newFileName+"\"";
            process->start(cmd);
        }
        else if(newFileName.split(".").last() == "wav")
        {
            QString cmd="ffmpeg -y -i \""+oldFileName+"\" \""+newFileName+"\"";
            process->start(cmd);
        }
        else if(newFileName.split(".").last() == "m4a")
        {
            QString cmd="ffmpeg -y -i \""+oldFileName+"\" \""+newFileName+"\"";
            process->start(cmd);
        }
        QFile::copy(oldFileName,newFileName);
    }
    else
    {
        if(QMessageBox::Yes)
        {
            qDebug()<<"确认";
            QFile::copy(oldFileName,newFileName);
        }
    }
    fileDialog->deleteLater();
    mBox->deleteLater();
    return ;

}

void MyThread::stop_btnPressed()//停止录音
{   
    audioInputFile->stop();//音频文件写入停止
    audioInputSound->stop();//监听停止
    file->close();
    audioRecorder->stop();
    qDebug()<<"成功停止";
    int ad =recordData->get("savedefault").toInt();
    int type=recordData->get("type").toInt();
    QString endPathStr;
    if(ad==1)
    {
        //弹存储为的框
//        saveas.show();//2020.11.12禁用此功能

        //QFileDialog指定父窗口,使系统弹窗在中间MainWindow::mutual->mainWid
        if(recordData->get("type").toInt()==1)
        {

            fileName = QFileDialog::getSaveFileName(
                              MainWindow::mutual->mainWid,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "Mp3(*.mp3)");
            qDebug()<<"获取"<<fileName;
            selectMp3();
        }
        else if(recordData->get("type").toInt()==2)
        {
            fileName = QFileDialog::getSaveFileName(
                              MainWindow::mutual->mainWid,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "M4a(*.m4a)");
            selectM4a();
        }
        else if(recordData->get("type").toInt()==3)
        {
            fileName = QFileDialog::getSaveFileName(
                              MainWindow::mutual->mainWid,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "Wav(*.wav)");
            selectWav();
        }
    }
    else
    {
        if(type==1)//1代表MP3
        {
            if( toConvertMp3( absolutionPath, (endFileName).toLocal8Bit().data())>0)
            {
                //如下5行代码后期重构时务必放入一个函数里...2021.01.15(重复使用的功能需放入同一函数中)
                qDebug()<<"*********************mp3";
                emit listItemAddSignal(endFileName,MainWindow::mutual->list->count()+1);
            }
        }
        else if(type==2)//2代表M4a
        {
            if( toConvertM4a( absolutionPath, (endFileName).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                emit listItemAddSignal(endFileName,MainWindow::mutual->list->count()+1);

            }
        }
        else if(type==3)//3代表Wav
        {
            if( toConvertWAV( absolutionPath, (endFileName).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                emit listItemAddSignal(endFileName,MainWindow::mutual->list->count()+1);

            }
        }
        else
        {

        }
        QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
        code->fromUnicode(fileName).data();

    }
    delete audioInputFile;
    audioInputFile = nullptr;
    delete audioInputSound;
    audioInputSound =nullptr;
    delete file;
    file = nullptr;

}

void MyThread::stop_saveDefault()//停止录音
{

    audioInputFile->stop();//音频文件写入停止
    audioInputSound->stop();//监听停止
    file->close();

    int type=recordData->get("type").toInt();
    QString endPathStr;
    QTime t1;
    t1=QTime::currentTime();
    QString str = t1.toString("hh:mm:ss");
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    default_Location = setDefaultPath(defaultPath);//设置默认存储录音的路径
    QDateTime current_time = QDateTime::currentDateTime();
    //显示时间，格式为：年-月-日 时：分：秒 周几,可自行定义QDateTime::currentDateTime().toString("yyyyMMddhhmmss")
    QString StrCurrentTime = current_time.toString("yyyy.MM.dd");
    fileName=StrCurrentTime;
    if(type==1)//1代表MP3
    {
        if( toConvertMp3( absolutionPath, (default_Location+tr("/")+fileName+tr("-")+str+tr(".mp3")).toLocal8Bit().data())>0)
        {
            //如下5行代码后期重构时务必放入一个函数里...2021.01.15(重复使用的功能需放入同一函数中)
            //改变配置文件中的存储路径
            endPathStr = default_Location+tr("/")+fileName+tr("-")+str+tr(".mp3");
            onChangeCurrentRecordList(endPathStr);//更新路径配置文件
            listItemAdd(endPathStr);
            updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用

        }
    }
    else if(type==2)//2代表M4a
    {
        if( toConvertM4a( absolutionPath, (default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a")).toLocal8Bit().data() ) > 0 )
        {
            //改变配置文件中的存储路径
            onChangeCurrentRecordList(default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a"));
            listItemAdd(default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a"));
            updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用

        }
    }
    else if(type==3)//3代表Wav
    {
        if( toConvertWAV( absolutionPath, (default_Location+tr("/")+fileName+tr("-")+str+tr(".wav")).toLocal8Bit().data() ) > 0 )
        {
            //改变配置文件中的存储路径
            onChangeCurrentRecordList(default_Location+tr("/")+fileName+tr("-")+str+tr(".wav"));
            listItemAdd(default_Location+tr("/")+fileName+tr("-")+str+tr(".wav"));
            updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用

        }
    }
    else
    {

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(fileName).data();

    delete audioInputFile;
    audioInputFile = nullptr;
    delete audioInputSound;
    audioInputSound =nullptr;
    delete file;
    file = nullptr;

}

//设置默认存储路径判断中英文环境下文件路径唯一
QString MyThread::setDefaultPath(QString path)
{
    QDir *record = new QDir;
    QLocale locale = QLocale::system().name();
    QString savePath;
    bool existEn = record->exists(path+"/recorder");
    bool existCH = record->exists(path+"/录音");
    //判断是否存在此路径,若两种路径都不存在就创建一个
    if(!existEn&&!existCH)
    {
        //创建record文件夹
        if(locale.language() == QLocale::English)
        {
            qDebug()<<"English Environment:"<<path+"/"+tr("recorder");
            record->mkdir(path+"/"+tr("recorder"));
        }
        else if(locale.language() == QLocale::Chinese)
        {
            qDebug()<<"中文环境:"<<path+"/"+tr("recorder");
            record->mkdir(path+"/"+tr("recorder"));
        }
        savePath = path+"/"+tr("recorder");
    }
    else
    {
        if(existEn)
        {
            savePath = path+"/recorder";
        }
        else if(existCH)
        {
            savePath = path+"/录音";
        }

    }
    return savePath;
}


void MyThread::updateAmplitudeList(int valueArray[])//更新检测到的音频振幅值到配置文件
{
    QString newStr="";
    for(int i=0;i<MYTHREAD_RECTANGLE_COUNT;i++)
    {
        QString amplitudeStr=recordData->get("amplitude").toString();//从配置文件中读取振幅集,配置文件中初始值为""
        QString str=QString::number(valueArray[i]);
        newStr=amplitudeStr+","+str;
        recordData->set("amplitude",newStr);
        //qDebug()<<newStr;
    }
    newStr+=";";
    recordData->set("amplitude",newStr);
    qDebug()<<"当前的振幅集合:"<<newStr;
}

QString MyThread::readPathCollected()
{
    QString str="";
    if (recordData != nullptr)
    {
        QStringList keyList = recordData->keys();
        if (keyList.contains("recorderpath"))
        {
            str=recordData->get("recorderpath").toString();
        }
    }
    return str;
}

void MyThread::writePathCollected(QString filePath)
{

    recordData->set("recorderpath",filePath);
}

int MyThread::readNumList()
{
    int num = 0;
    if (recordData != nullptr) {
        QStringList keyList = recordData->keys();
        if (keyList.contains("num"))
        {
            num = recordData->get("num").toInt();
        }
    }
    return num;
}

void MyThread::writeNumList(int num)
{
    recordData->set("num",num);
}

void MyThread::onChangeCurrentRecordList(QString filePath)
{
    QString newFilePath="";
    QString strSaveFilePath=readPathCollected();//从配置文件中读取路径集,配置文件中初始值为""
    newFilePath=strSaveFilePath+","+filePath;
    //qDebug()<<newFilePath;
    writeNumList(newFilePath.split(",").length());
    writePathCollected(newFilePath);
}

void MyThread::pauseRecord()//暂停录制
{
    audioInputFile->stop();
    audioInputSound->stop();
    audioRecorder->pause();
}

void MyThread::playRecord()//开始录制
{

    audioInputFile->start(file);
//    inputDevSound = audioInputSound->start();
    audioRecorder->record();
//    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));//点击开始三角按钮时录制
// connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));
}

void MyThread::InitMonitor()
{

    //判断当前的音频输入设备是否支持QAudioFormat配置，如果不支持，获取支持的最接近的配置信息
//    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());//记得改回来Input
//    if (!infoIn.isFormatSupported(format))
//    {
//        //默认不支持时找最近设备
//        format = infoIn.nearestFormat(format);
//        //qDebug()<<mFormatSound;
//    }

//    qDebug()<<infoIn.deviceName()<<mFormatSound;
    //CreateAudioInput();
    //************************************************2020.9.24
    outputDevSound=nullptr;
    inputDevSound=nullptr;
    audioInputSound=nullptr;
    audioOutputSound=nullptr;
    if (inputDevSound != 0) {
        disconnect(inputDevSound, 0, this, 0);
        inputDevSound = 0;

    }

    int i=recordData->get("source").toInt();
    monitorVoiceSource(i);

//    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));
//    connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));
}

QAudioDeviceInfo MyThread::monitorVoiceSource(int i)
{
    qDebug()<<"配置文件中的值(1.全部2.内部3.麦克风)"<<i;
    QStringList deviceList;
//    for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
//    {
//        deviceList += deviceInfo.deviceName();
////         qDebug()<<"当前输入设备:"<<deviceInfo.deviceName()<<mFormatSound;//可以判断当前输入设备
//    }
    if(i==2)//系统内部
    {
        //当录制系统内部声音时吊用此方法
        qDebug()<<"内部"<<i;
        for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
        {
            deviceList += deviceInfo.deviceName();
            if(deviceInfo.deviceName().contains("alsa_output"))
            {
                QAudioDeviceInfo inputDevice(deviceInfo);
                mFormatSound = inputDevice.nearestFormat(mFormatSound);
                audioInputSound = new QAudioInput(inputDevice, mFormatSound, this);
//                inputDevSound = audioInputSound->start();
                qDebug()<<"当前内部输入设备:"<<inputDevice.deviceName()<<mFormatSound;//可以判断当前输入设备
                return inputDevice;
            }


        }

//        QStringList deviceList;
//        for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
//        {
//            deviceList += deviceInfo.deviceName();
//            if(deviceInfo.deviceName().contains("alsa_output"))
//            {
//                audioInputFile = new QAudioInput(deviceInfo, format, this);
//                qDebug()<<"您选择了录制系统内部设备:"<<deviceInfo.deviceName()<<format;//可以检测系统内部声音
//                return deviceInfo;
//            }
//            else
//            {
////                qDebug()<<deviceInfo.deviceName();
//            }

//        }

    }
    else//其他
    {
        QAudioDeviceInfo inputDevice(QAudioDeviceInfo::defaultInputDevice());
        mFormatSound = inputDevice.nearestFormat(mFormatSound);
        audioInputSound = new QAudioInput(inputDevice, mFormatSound, this);
//        inputDevSound = audioInputSound->start();
        qDebug()<<"当前外部输入设备:"<<inputDevice.deviceName()<<mFormatSound;//可以判断当前输入设备
        return inputDevice;
    }

}

//调节音量大小
void MyThread::OnSliderValueChanged(int value)
{
    soundVolume = value;
    emit changeVoicePicture();

}

void MyThread::OnReadMore()
{
    if(!canMonitor)return;//
    canMonitor=false;
    //如果输入为空则返回空
    if(!audioInputSound)
    {
        qDebug()<<" REISIVE VOICE";
        return;
    }
    //qDebug()<<"检测到声音";
    //处理音频输入，_Buffer中保存音频数据，len是获取到的数据的长度
    QByteArray _Buffer(BufferSize, 0);
    len = audioInputSound->bytesReady();
    if(len > 4096)
        len = 4096; 
    qint64 l = inputDevSound->read(_Buffer.data(), len);
    if(l <= 0) {
        return;
    }
    short* tempData = (short*)_Buffer.data();
    outdata = tempData;
    MaxValue = 0;
    for (int i=0; i<len;i++ )
    {
        //把样本数据转换为整型
        value = abs(useVolumeSample(outdata[i]));/*麦克风中的薄膜始终是在平衡位置附近value会检测到正负相间的震荡频率,加个绝对值*/
        MaxValue = MaxValue>=value ? MaxValue : value;
    }
//    MainWindow::mutual->valueArray[count] = MaxValue;//2020.11.12暂时禁用循环存值
//    count++;//2020.11.12暂时禁用循环存值
//    count=count%MYTHREAD_RECTANGLE_COUNT;//2020.11.12暂时禁用循环存值
    emit recordPaint(MaxValue);//每检测到value就发送一次绘图信号
}

int MyThread::useVolumeSample(qreal sample)
{
    return qMax(qMin(((sample * soundVolume) / 100) ,30000.0),-30000.0);//求每次检测到的振幅组中最大值
}

QString MyThread::listItemAdd(QString filePath)//注意当首次添加文件时，配置文件中路径为空
{
    QStringList keyList = recordData->keys();
    QStringList listRecordPath = readPathCollected().split(",");
    QStringList listAmplitude = recordData->get("amplitude").toString().split(";");//波形图配置文件;
    if (keyList.contains("recorderpath"))
    {
        int m=readNumList();
        qDebug()<<"这时的数量:"<<m;
        qDebug()<<filePath;
        for(int i=1;i<m;i++)
        {
            QString str="";
            str = listRecordPath.at(i);
            QFileInfo fileinfo(str);
            QString filesuffix = fileinfo.suffix();//判断文件后缀
//          qDebug()<<fileinfo.isFile();//判断是否为文件，是文件就存在了,因为在本地删除后，同步文件列表下才打开时那个文件也没了
            if(!str.contains(filePath)||MainWindow::mutual->list->count()==0)//不包含有2种情况：1.路径中有不同的文件名 2.文件路径本身不存在或被删除
            {
                if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
                {
                    emit listItemAddSignal(filePath,MainWindow::mutual->list->count()+1);
                    break;//因为点击保存一次就添加一次所以break
                }
                else
                {
                    qDebug()<<str<<"listItemAdd:文件或被删除！";
                    QString subStr = ","+str;//子串
                    QString subAmplitudeStr = listAmplitude.at(i);//子振幅串
                    /*
                     * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串以及振幅串内容
                    */
                    QString oldStr = recordData->get("recorderpath").toString();
                    int pos = oldStr.indexOf(subStr);
                    QString oldAmplitudeStr = recordData->get("amplitude").toString();
                    int posAmplitude = oldAmplitudeStr.indexOf(subAmplitudeStr);
                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    writePathCollected(newStr);//更新路径串
                    QString newAmplitudeStr = oldAmplitudeStr.remove(posAmplitude,subAmplitudeStr.length()+1);
                    qDebug()<<"新振幅串"<<newAmplitudeStr;
//                    updateAmplitudeList(MainWindow::mutual->valueArray);
                    recordData->set("amplitude",newAmplitudeStr);//更新振幅串
                    writeNumList(readNumList()-1);
                }

            }
            else
            {
                emit listItemAddSignal(filePath,MainWindow::mutual->list->count()+1);
                break;//因为点击保存一次就添加一次所以break
            }

        }

    }

    return filePath;

}

QString MyThread::isSameFileName(QString FileName)
{
    QString StrCurrentTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    qDebug()<<"文件路径:"<<FileName<<"唯一性标识:"<<StrCurrentTime;
    QStringList keyList = recordData->keys();
    if (keyList.contains("recorderpath"))
    {
        int fileName_addNum;
        QStringList listRecordPath=readPathCollected().split(",");
        qDebug()<<"路径集里此路径:"<<listRecordPath.contains(FileName)<<"索引"<<listRecordPath.indexOf(FileName);
        if(listRecordPath.contains(FileName))
        {
            fileName_addNum = recordData->get("samefilenum").toInt();
            int position = FileName.lastIndexOf(".");//从后面开始查"."所在位置
            qDebug()<<"从后面开始查.所在位置"<<position;
            QString newFileName = FileName.insert(position,"["+ QString::number(fileName_addNum) +"]");
            qDebug()<<"新文件名"<<newFileName;
            fileName_addNum++;
            recordData->set("samefilenum",fileName_addNum);
            return newFileName;
        }

    }
    return FileName;

}

void MyThread::selectMp3()
{

    if(QMessageBox::Ok && fileName.length() == 0||fileName.contains(" ")||fileName.contains("?")||fileName.contains("'")||fileName.contains("\"")||fileName.contains("\\"))
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),tr("Do not enter illegal file name"));
        return ;
    }
    QString filename = fileName.mid(fileName.lastIndexOf("/") +1);
    QString s = ".";
    QString first_s = filename.at(0);
    if(first_s == s)
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                             tr("Please do not name the file with . at the beginning!"));
        return;
    }
    else
    {
        QString Home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if(!fileName.contains(Home_path,Qt::CaseSensitive))
        {
            qDebug()<<"保存失败";
            return;
        }
        else
        {
            if( toConvertMp3( absolutionPath, (fileName+tr(".mp3")).toLocal8Bit().data() ) > 0 )
            {
                //添加前要判断是否重复文件名
                onChangeCurrentRecordList(endFileName);
                listItemAdd(endFileName);
                updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用
                QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + endFileName);
            }
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(endFileName).data();
    if (endFileName.isEmpty())
    {

        return ;
    }
    else
    {

        //fileName += ".mp3";
        qDebug() << "filenamePath=" << endFileName;

    }
    return ;

}
void MyThread::selectM4a()
{
    if(QMessageBox::Ok && fileName.length() == 0||fileName.contains(" ")||fileName.contains("?")||fileName.contains("'")||fileName.contains("\"")||fileName.contains("\\"))
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),tr("Do not enter illegal file name"));
        return ;
    }
    QString filename = fileName.mid(fileName.lastIndexOf("/") +1);
    QString s = ".";
    QString first_s = filename.at(0);
    if(first_s == s)
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                             tr("Please do not name the file with . at the beginning!"));
        return;
    }
    else
    {
        QString Home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if(!fileName.contains(Home_path,Qt::CaseSensitive))
        {
            qDebug()<<"保存失败";
            return;
        }
        else
        {
            if( toConvertM4a( absolutionPath, (fileName+tr(".m4a")).toLocal8Bit().data() ) > 0 )
            {
                //添加前要判断是否重复文件名
                onChangeCurrentRecordList(endFileName);
                listItemAdd(endFileName);
                updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用
                QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + endFileName);
            }
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(endFileName).data();
    if (endFileName.isEmpty())
    {

        return ;
    }
    else
    {

        qDebug() << "filenamePath=" << endFileName;

    }
    return ;

}
void MyThread::selectWav()
{
    if(QMessageBox::Ok && fileName.length() == 0||fileName.contains(" ")||fileName.contains("?")||fileName.contains("'")||fileName.contains("\"")||fileName.contains("\\"))
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),tr("Do not enter illegal file name"));
        return ;
    }
    QString filename = fileName.mid(fileName.lastIndexOf("/") +1);
    QString s = ".";
    QString first_s = filename.at(0);
    if(first_s == s)
    {
        QMessageBox::warning(MainWindow::mutual->mainWid,tr("Warning"),
                             tr("Please do not name the file with . at the beginning!"));
        return;
    }
    else
    {
        QString Home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if(!fileName.contains(Home_path,Qt::CaseSensitive))
        {
            qDebug()<<"保存失败";
            return;
        }
        else
        {

            if( toConvertWAV(absolutionPath, (fileName+tr(".wav")).toLocal8Bit().data() ) > 0 )
            {
                //添加前要判断是否重复文件名
                onChangeCurrentRecordList(endFileName);
                listItemAdd(endFileName);
                updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用
                QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + endFileName);
            }
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(endFileName).data();
    if (endFileName.isEmpty())
    {

        return ;
    }
    else
    {

        qDebug() << "filenamePath=" << endFileName;

    }

    return ;
}
void MyThread::test()
{
    qDebug()<<"MyThread::test";
}

void MyThread::processBuffer(const QAudioBuffer &buffer)
{
    if(!canMonitor) return;
    canMonitor = false;

    QVector<qreal> levels = getBufferLevels(buffer);
        qDebug()<<"长度:"<<levels.count();
    MaxValue = 0;
    for (int i = 0; i < levels.count(); i++) {
        qDebug()<<"振幅:"<<levels.at(i)*10000;
        //把样本数据转换为整型
        value = abs(useVolumeSample(levels.at(i)*10000));/*麦克风中的薄膜始终是在平衡位置附近value会检测到正负相间的震荡频率,加个绝对值*/
        MaxValue = MaxValue>=value ? MaxValue : value;
    }
//    MainWindow::mutual->valueArray[count] = 0;//2020.11.12暂时禁用循环存值
    count++;//2020.11.12暂时禁用循环存值
    count=count%MYTHREAD_RECTANGLE_COUNT;//2020.11.12暂时禁用循环存值
    emit recordPaint(MaxValue);//每检测到value就发送一次绘图信号

}

//获得buffer等级
QVector<qreal> MyThread::getBufferLevels(const QAudioBuffer &buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

// This function returns the maximum possible sample value for a given audio format
qreal MyThread::getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

template <class T>
QVector<qreal> MyThread::getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

