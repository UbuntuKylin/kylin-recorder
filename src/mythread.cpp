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
 *  Authors: baijincheng <baijincheng@kylinos.cn>
 */
#include "mythread.h"
#include "mainwindow.h"

struct WAVFILEHEADER
{
    char RiffName[4];// RIFF 头(标志)
    uint32_t RiffLength;//从下一个开始到文件结尾的字节数
    char WavName[4];// WAVE 文件
    char FmtName[4];// fmt 波形格式
    uint32_t FmtLength;// 数据格式 16LE,32LE等等
    uint16_t AudioFormat;//音频格式是否是压缩的PCM编码，1无压缩，大于1有压缩
    uint16_t ChannleNumber;//声道数量
    uint32_t SampleRate;//采样频率
    uint32_t BytesPerSecond;//byte 率， byterate = 采样频率 * 音频通道数量 * 数据位数
    uint16_t BytesPerSample;//块对齐/帧大小 framesize = 通道数 * 数据位数
    uint16_t BitsPerSample; //样本数据位数

    char DATANAME[4];//catheFileName , QString wavF数据块中的块头
    uint32_t DataLength;//实际音频数据的大小

};

MyThread::MyThread(QWidget *parent) : QMainWindow(parent)
{

    qDebug()<<"子线程MyThread::startThreadSlot QThread::currentThreadId()=="<<QThread::currentThreadId();
    pTimer = new QTimer;
    audioInputSound = nullptr;
    audioOutputSound = nullptr;
    inputDevSound = nullptr;
    recordData = new QGSettings(KYLINRECORDER);

    my_time=new QTimer;//延时检测
    process = new QProcess;//调用外部程序如：ffmpeg进行音频文件转码
    connect(process, SIGNAL(finished(int)), this, SLOT(audioConversionFinish(int)), Qt::UniqueConnection);
    connect(my_time,&QTimer::timeout,this,[=]{
        canMonitor=true;
    });
    connect(this,&MyThread::listItemAddSignal,MainWindow::mutual,&MainWindow::slotListItemAdd);
    connect(this,&MyThread::handling,MainWindow::mutual,&MainWindow::handlingSlot);//处理中...

}

//pcm转换wav
qint64 MyThread::addWavHeader(QString catheFileName , QString filename)
{
    WAVFILEHEADER WavFileHeader;
    qstrcpy(WavFileHeader.RiffName, "RIFF");
    qstrcpy(WavFileHeader.WavName, "WAVE");
    qstrcpy(WavFileHeader.FmtName, "fmt ");
    qstrcpy(WavFileHeader.DATANAME, "data");
    WavFileHeader.FmtLength = 16;  //  表示 FMT 的长度
    WavFileHeader.AudioFormat = 1; //这个表示 PCM 编码无压缩;

    WavFileHeader.BitsPerSample = 16;//数据位数
    WavFileHeader.BytesPerSample = 2;
    WavFileHeader.SampleRate = 48000;//采样频率
    WavFileHeader.BytesPerSecond = 96000;//byte 率， byterate = 采样频率 * 音频通道数量
    WavFileHeader.ChannleNumber = 1;//音频通道数WAV要平声道

    QFile cacheFile(catheFileName);
    QFile wavFile(filename);
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
//    WavFileHeader.RiffLength = nFileLen - 8 + nSize;
//    WavFileHeader.DataLength = nFileLen;

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((char *)&WavFileHeader, nSize);
    wavFile.write(cacheFile.readAll());

    cacheFile.close();
    wavFile.close();
    return nFileLen;
}

//pcm转换mp3
qint64 MyThread::toConvertMp3(QString catheFileName , QString mp3FileName)
{
    endFileName = isSameFileName(mp3FileName);//判断是否文件名重复
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
    //ffmpeg -y -f s16le -ar 16k -ac 1 -i input.raw output.mp3//ffmpeg命令真香
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
   // code->fromUnicode(catheFileName).data();
    code->fromUnicode(endFileName).data();


    QString cmd="ffmpeg -y -f s16le -ar 48k -ac 2 -i \""+catheFileName+"\" \""+endFileName+"\"";
//    qDebug()<<"******"<<catheFileName<<"*"<<cmd<<"******";
    process->start(cmd);
    QString str = "处理中...";
    emit handling(str);
    process->waitForFinished();


    cacheFile.close();
    mp3File.close();

    return nFileLen;
}

qint64 MyThread::toConvertM4a(QString catheFileName , QString m4aFileName)
{
    QFile cacheFile(catheFileName);
    QFile m4aFile(m4aFileName);
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
    code->fromUnicode(m4aFileName).data();
    QString cmd="ffmpeg -y -f s16le -ar 48k -ac 2 -i \""+catheFileName+"\" \""+m4aFileName+"\"";
    //qDebug()<<"******"<<catheFileName<<"*"<<cmd<<"******";
    process->start(cmd);
    QString str = "处理中...";
    emit handling(str);
    process->waitForFinished();

    cacheFile.close();
    m4aFile.close();

    return nFileLen;
}

void MyThread::audioConversionFinish(int isOk)
{
    if(isOk==0)
    {
        qDebug() << "音频格式转换成功"<<isOk;
        isSuccess=isOk;

        //QMessageBox::information(this, "提示信息", "音频格式转换成功！", QMessageBox::Default);

    }
    else
    {
        qDebug() << "失败"<<isOk;
        isSuccess=isOk;
        //QMessageBox::critical(this, "错误信息", "音频格式转换失败！", QMessageBox::Default);
    }

}

//按压录音按钮之后触发
void MyThread::record_pressed()
{
    qDebug()<<"开始录音:";

    //开始前，count先从0开始
    count=0;
    my_time->start(150);//每隔xms检测一次，波形图缓慢刷新，这行代码一定在前面

    if(recordData->get("type").toInt()==1)//mp3
    {
        format=Mp3();
    }
    else if(recordData->get("type").toInt()==3)//wav
    {
        format=Wav();
        //qDebug()<<audioInputFile;
    }
    else
    {
//        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();//获取设备信息
//        if(!info.isFormatSupported(format))
//        {
//            format = info.nearestFormat(format);
//        }
        format=M4a();

    }

    InitMonitor();
    file =new QFile();
    file->setFileName("record.raw");
    bool is_open =file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(!is_open)
    {
        qDebug()<<"打开失败";
        exit(1);
    }

    //    QAudioDeviceInfo info = monitorVoiceSource(i);
    //    for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    //    {
    //        if(deviceInfo.deviceName().contains("alsa_output"))
    //        {
    //            QAudioDeviceInfo inputDevice(deviceInfo);
    //            mFormatSound = inputDevice.preferredFormat();
    //            qDebug()<<"写入文件的输入设备:"<<deviceInfo.deviceName()<<mFormatSound;
    //            break;
    //        }
    //    }

    audioInputFile = new QAudioInput(format, this);
    qDebug()<<"录音开始";
    audioInputFile->start(file);




}
//Mp3格式
QAudioFormat MyThread::Mp3()
{
    QAudioFormat format;
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，48000HZ(标准CD级)，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(2);//立体声，数目为2
    format.setSampleSize(16);
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//低位优先
    format.setSampleType(QAudioFormat::SignedInt);//v10.1规定QAudioFormat::SignedInt
    return format;
}
//格式Wav格式
QAudioFormat MyThread::Wav()
{
    QAudioFormat format;
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(1);//平声道，数目为1
    format.setSampleSize(16);
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//低位优先
    format.setSampleType(QAudioFormat::SignedInt);//v10.1规定QAudioFormat::SignedInt。
    return format;
}

//格式M4a格式
QAudioFormat MyThread::M4a()
{
    QAudioFormat format;
    format.setSampleRate(48000);/*
                                *每秒钟对声音的采样次数，越大越精细，
                                *48000HZ的采样率(每秒钟采集48000个声波的点)
                                */
    format.setChannelCount(2);//平声道，数目为1
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
    QString newFileName = QFileDialog::getSaveFileName(this,
                                                     tr("Select a file storage directory"),
                                                     defaultPath+"/RecordFile.mp3",
                                                     fileType);
    if(newFileName =="")
    {
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
        //QFile::copy(oldFileName,newFileName);
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
    return ;

}

void MyThread::stop_btnPressed()//停止录音
{

    qDebug()<<"更新————————————————————————0";
    audioInputFile->stop();//音频文件写入停止
    qDebug()<<"更新————————————————————————1";
    audioInputSound->stop();//监听停止
    qDebug()<<"更新————————————————————————2";
    file->close();
    qDebug()<<"更新————————————————————————3";

    updateAmplitudeList(MainWindow::mutual->valueArray);//更新振幅列表//2020.11.12暂时禁用
//        if(tmpArray1.length()<110)
//        {
//            QList<int> tmpArray3;
//            int geshu = 110/tmpArray1.length();
//            qDebug()<<tmpArray1.length()<<" "<<geshu;
//            int tag=0;
//            for(int i=0;i<110;i++)
//            {
//                int temp=i;
//                if(temp%geshu!=0)
//                {
//                    tmpArray3.append(tmpArray1.at(tag));
//                }
//                else
//                {
//                    if(tag<geshu)
//                    {
//                        tag++;
//                        tmpArray3.append(tmpArray1.at(tag));
//                    }
//                    //tmpArray3.append(0);
//                }
//            }
//            tmpArray1=tmpArray3;
//            updateAmplitudeList(tmpArray1);//更新振幅列表//2020.11.12暂时禁用
//        }
//        else
//        {
//            updateAmplitudeList(tmpArray1);//更新振幅列表//2020.11.12暂时禁用
//        }

    qDebug()<<"成功停止";
    int ad =recordData->get("savedefault").toInt();
    int type=recordData->get("type").toInt();
    QString endPathStr;
    if(ad==1)
    {
        //弹存储为的框
//        saveas.show();//2020.11.12禁用此功能
        if(recordData->get("type").toInt()==1)
        {
            fileName = QFileDialog::getSaveFileName(
                              this,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "Mp3(*.mp3)");
            selectMp3();
        }
        else if(recordData->get("type").toInt()==2)
        {
            fileName = QFileDialog::getSaveFileName(
                              this,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "M4a(*.m4a)");
            selectM4a();
        }
        else if(recordData->get("type").toInt()==3)
        {
            fileName = QFileDialog::getSaveFileName(
                              this,
                              tr("Select a file storage directory"),
                                  QDir::currentPath(),
                                  "Wav(*.wav)");
            selectWav();
        }
    }
    else
    {
        QTime t1;
        t1=QTime::currentTime();
        QString str = t1.toString("hh:mm:ss");
//        QString str = QString::number(t1.hour())+":"+QString::number(t1.minute())+":"+QString::number(t1.second());
        default_Location = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        QDateTime current_time = QDateTime::currentDateTime();
        //显示时间，格式为：年-月-日 时：分：秒 周几,可自行定义QDateTime::currentDateTime().toString("yyyyMMddhhmmss")
        QString StrCurrentTime = current_time.toString("yyyy.MM.dd");
        fileName=StrCurrentTime;
        if(type==1)//1代表MP3
        {
            if( toConvertMp3( "record.raw", (default_Location+tr("/")+fileName+tr("-")+str+tr(".mp3")).toLocal8Bit().data())>0)
            {
                //如下5行代码后期重构时务必放入一个函数里...2021.01.15(重复使用的功能需放入同一函数中)
                //改变配置文件中的存储路径
                endPathStr = default_Location+tr("/")+fileName+tr("-")+str+tr(".mp3");
                onChangeCurrentRecordList(endPathStr);//更新路径配置文件
                listItemAdd(endPathStr);
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ default_Location+tr("/")+fileName+tr("-")+str, QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
            }
        }
        else if(type==2)//2代表M4a
        {
            if( toConvertM4a( "record.raw", (default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a")).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                onChangeCurrentRecordList(default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a"));
                listItemAdd(default_Location+tr("/")+fileName+tr("-")+str+tr(".m4a"));
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ default_Location+tr("/")+fileName+tr("-")+str, QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
            }
        }
        else if(type==3)//3代表Wav
        {
            if( addWavHeader( "record.raw", (default_Location+tr("/")+fileName+tr("-")+str+tr(".wav")).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                onChangeCurrentRecordList(default_Location+tr("/")+fileName+tr("-")+str+tr(".wav"));
                listItemAdd(default_Location+tr("/")+fileName+tr("-")+str+tr(".wav"));
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ default_Location+tr("/")+fileName+tr("-")+str, QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
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
    qDebug()<<newStr;
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


}

void MyThread::playRecord()//开始录制
{
    audioInputFile->start(file);
    //outputDevSound = audioOutputSound->start();
    inputDevSound = audioInputSound->start();
    //qDebug()<<mpAudioInputSound->volume();
    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));//点击开始三角按钮时录制
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


    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));

}

QAudioDeviceInfo MyThread::monitorVoiceSource(int i)
{
    qDebug()<<"配置文件中的值(1.全部2.内部3.麦克风)"<<i;
    QStringList deviceList;
    for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        deviceList += deviceInfo.deviceName();
//         qDebug()<<"当前输入设备:"<<deviceInfo.deviceName()<<mFormatSound;//可以判断当前输入设备
    }
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
                inputDevSound = audioInputSound->start();
                qDebug()<<"当前输入设备:"<<inputDevice.deviceName()<<mFormatSound;//可以判断当前输入设备
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
        inputDevSound = audioInputSound->start();
        qDebug()<<"当前输入设备:"<<inputDevice.deviceName()<<mFormatSound;//可以判断当前输入设备
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
//    qDebug()<<"检测到声音";
    //处理音频输入，_Buffer中保存音频数据，len是获取到的数据的长度
    QByteArray _Buffer(BufferSize, 0);
    len = audioInputSound->bytesReady();
    if(len > 4096)
        len = 4096; 
    qint64 l = inputDevSound->read(_Buffer.data(), len);
    if(l <= 0) return;
        short* tempData = (short*)_Buffer.data();
        outdata=tempData;
        MaxValue = 0;
        for (int i=0; i<len;i++ )
        {
            //把样本数据转换为整型
            value = abs(useVolumeSample(outdata[i]));/*麦克风中的薄膜始终是在平衡位置附近                                                 value会检测到正负相间的震荡频率,加个绝对值*/
            MaxValue = MaxValue>=value ? MaxValue : value;
        }

//        if(beishu==1)
//        {
//            tmpArray1.append(MaxValue);
//            xianzhi++;
//            if(xianzhi==110)//当主队列填满后，2倍
//            {
//                xianzhi=0;
//                beishu=beishu*2;
//            }
//        }
//        else
//        {
//            quzhi++;//隔quzhi个取一个填入分队列
//            if(quzhi==beishu/2)
//            {
//                tmpArray2.append(MaxValue);
//                xianzhi++;
//                quzhi=0;
//            }
//            if(xianzhi==110)
//            {
//                quzhi=0;
//                xianzhi=0;
//                beishu=beishu*2;
//                QList<int> tmpArray3;
//                for(int i=0;i<110;i++)
//                {
//                    if(i%2)
//                        tmpArray3.append(tmpArray1.at(i));
//                    else
//                        tmpArray3.append(tmpArray2.at(i));
//                }
//                tmpArray1=tmpArray3;
//                tmpArray2.clear();
//            }
//        }
//qDebug()<<"==========="<<MaxValue;
        MainWindow::mutual->valueArray[count] = MaxValue;//2020.11.12暂时禁用循环存值
        count++;//2020.11.12暂时禁用循环存值

        count=count%MYTHREAD_RECTANGLE_COUNT;//2020.11.12暂时禁用循环存值
        emit recordPaint(MaxValue);//每检测到value就发送一次绘图信号
}

int MyThread::useVolumeSample(short sample)
{
    return qMax(qMin(((sample * soundVolume) / 100) ,30000),-30000);//求每次检测到的振幅组中最大值
}

QString MyThread::listItemAdd(QString filePath)//注意当首次添加文件时，配置文件中路径为空
{
    QStringList keyList = recordData->keys();
    QStringList listRecordPath = readPathCollected().split(",");
    QStringList listAmplitude = recordData->get("amplitude").toString().split(";");//波形图配置文件;
    qDebug()<<"路径集:"<<listRecordPath<<"振幅组:"<<listAmplitude;
    if (keyList.contains("recorderpath"))
    {
        int  m=readNumList();
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
                    if(readPathCollected()=="")//首次添加时配置文件一定是空的所以fileinfo.isFile()为false
                    {
                        emit listItemAddSignal(filePath,MainWindow::mutual->list->count()+1);
                        return filePath;
                    }
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
                    recordData->set("amplitude",newAmplitudeStr);//更新振幅串
                    writeNumList(readNumList()-1);
                    qDebug()<<"**********************";
                }

            }
            qDebug()<<"**********************"<<i;

        }

    }

    return filePath;

}

QString MyThread::isSameFileName(QString FileName)
{
    qDebug()<<FileName;
    QStringList keyList = recordData->keys();
    if (keyList.contains("recorderpath"))
    {
        int  m=readNumList()-1;
        //qDebug()<<m;
        QStringList listRecordPath=readPathCollected().split(",");
        //qDebug()<<listRecordPath;
        QString tempFileName;
        QString oldFileName;
        QString str;
        int fileName_addNum;
        for(int i=0;i<m;i++)
        {
            str=listRecordPath.at(i+1);
            //qDebug()<<str;
            QFileInfo fileinfo(str);
            QString filesuffix = fileinfo.suffix();//判断文件后缀

//          qDebug()<<fileinfo.isFile();//判断是否为文件，是文件就存在了,因为在本地删除后，同步文件列表下才打开时那个文件也没了
            //qDebug()<<filesuffix;
//            fileName.split("/").last()
            if(str.contains(FileName))
            {
                if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
                {
                    qDebug()<<"绝对路径名相同！文件名后加(i)";
                    fileName_addNum = recordData->get("samefilenum").toInt();
                    tempFileName = FileName.split("/").last();
                    oldFileName = tempFileName.split(".").first();

                    QString newFileName = oldFileName+"("+ QString::number(fileName_addNum) +")";
                    qDebug()<<newFileName;
                    int pos = str.indexOf(oldFileName);
                    QString tempStr = str.remove(pos,oldFileName.length());
                    QString newStr = tempStr.insert(pos,newFileName);
                    qDebug()<<newStr;
                    fileName_addNum++;
                    recordData->set("samefilenum",fileName_addNum);
                    return newStr;

                }
            }
        }

    }
    return FileName;

}

void MyThread::selectMp3()
{
    if(fileName.length() == 0)
    {
        WrrMsg = new QMessageBox(QMessageBox::Warning, tr("Warning"), tr("You have not selected any storage location!"), QMessageBox::Ok );
        WrrMsg->button(QMessageBox::Ok)->setText(tr("OK"));
        WrrMsg->exec();
    }
    else
    {
        if( toConvertMp3( "record.raw", (fileName+tr(".mp3")).toLocal8Bit().data() ) > 0 )
        {

            //添加前要判断是否重复文件名
            listItemAdd(endFileName);
            onChangeCurrentRecordList(endFileName);
            QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + endFileName);
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



}
void MyThread::selectM4a()
{
    if(fileName.length() == 0) {
        QMessageBox::information(NULL, tr("filename"), tr("You didn't select any files."));

    } else {
        if( toConvertM4a( "record.raw", (fileName+tr(".m4a")).toLocal8Bit().data() ) > 0 )
        {

            listItemAdd(fileName+tr(".m4a"));
            onChangeCurrentRecordList(fileName+tr(".m4a"));
            QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + fileName);
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(fileName).data();
    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << "filenamePath=" << fileName;
        fileName += ".m4a";
    }

}
void MyThread::selectWav()
{
    if(fileName.length() == 0) {
        QMessageBox::information(NULL, tr("filename"), tr("You didn't select any files."));

    } else {
        if( addWavHeader( "record.raw", (fileName+tr(".wav")).toLocal8Bit().data() ) > 0 )
        {

            listItemAdd(fileName+tr(".wav"));
            onChangeCurrentRecordList(fileName+tr(".wav"));
            QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + fileName);
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(fileName).data();
    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << "filenamePath=" << fileName;
        fileName += ".wav";
    }

}
void MyThread::test()
{
    qDebug()<<"MyThread::test";
}

