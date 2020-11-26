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

    qDebug()<<"MyThread::startThreadSlot QThread::currentThreadId()=="<<QThread::currentThreadId();
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
    QFile cacheFile(catheFileName);
    QFile mp3File(mp3FileName);
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

        qDebug() <<ba.size()<< nFileLen ;
    //ffmpeg -y -f s16le -ar 16k -ac 1 -i input.raw output.mp3//ffmpeg命令真香
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
   // code->fromUnicode(catheFileName).data();
    code->fromUnicode(mp3FileName).data();
    QString cmd="ffmpeg -y -f s16le -ar 48k -ac 2 -i \""+catheFileName+"\" \""+mp3FileName+"\"";
    //qDebug()<<"******"<<catheFileName<<"*"<<cmd<<"******";
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
    //ffmpeg -y -f s16le -ar 16k -ac 1 -i input.raw output.mp3//ffmpeg命令真香
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
   // code->fromUnicode(catheFileName).data();
    code->fromUnicode(m4aFileName).data();
    QString cmd="ffmpeg -y -f s16le -ar 8k -ac 1 -i \""+catheFileName+"\" \""+m4aFileName+"\"";
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
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();//获取设备信息
        if(!info.isFormatSupported(format))
        {
            format = info.nearestFormat(format);
        }

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
void MyThread::stop_btnPressed()//停止录音
{

    audioInputFile->stop();//音频文件写入停止
    audioInputSound->stop();//监听停止
//    audioOutputFile->stop();//音频输出停止
//    audioOutputSound->stop();//监听停止
    file->close();

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
        //qDebug()<<seq;
        QTime t1;
        t1=QTime::currentTime();
        QString str = QString::number(t1.hour())+":"+QString::number(t1.minute())+":"+QString::number(t1.second());
        desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QDateTime current_time = QDateTime::currentDateTime();
        //显示时间，格式为：年-月-日 时：分：秒 周几,可自行定义
        QString StrCurrentTime = current_time.toString("yyyy.MM.dd");
        fileName=StrCurrentTime;
        if(type==1)//1代表MP3
        {
            if( toConvertMp3( "record.raw", (desktop_path+tr("/")+fileName+tr("-")+str+tr(".mp3")).toLocal8Bit().data())>0)
            {
                //MainWindow::mutual->playerTotalTime(desktop_path+tr("/")+fileName+tr("-")+str+tr(".mp3"));
                //qDebug()<<"Yes";
                //改变配置文件中的存储路径
                //qDebug()<<desktop_path+tr("/")+fileName+tr("-")+str+tr(".mp3");
                onChangeCurrentRecordList(desktop_path+tr("/")+fileName+tr("-")+str+tr(".mp3"));
                listItemAdd(desktop_path+tr("/")+fileName+tr("-")+str+tr(".mp3"));
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ fileName+tr("-")+str, QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
            }
        }
        else if(type==2)//2代表M4a
        {
            if( toConvertM4a( "record.raw", (desktop_path+tr("/")+fileName+tr("-")+str+tr(".m4a")).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                onChangeCurrentRecordList(desktop_path+tr("/")+fileName+tr("-")+str+tr(".m4a"));
                listItemAdd(desktop_path+tr("/")+fileName+tr("-")+str+tr(".m4a"));
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ fileName+tr("-")+str, QMessageBox::Yes );
                WrrMsg->button(QMessageBox::Yes)->setText(tr("OK"));
                WrrMsg->exec();
            }
        }
        else if(type==3)//3代表Wav
        {
            if( addWavHeader( "record.raw", (desktop_path+tr("/")+fileName+tr("-")+str+tr(".wav")).toLocal8Bit().data() ) > 0 )
            {
                //改变配置文件中的存储路径
                onChangeCurrentRecordList(desktop_path+tr("/")+fileName+tr("-")+str+tr(".wav"));
                listItemAdd(desktop_path+tr("/")+fileName+tr("-")+str+tr(".wav"));
                WrrMsg = new QMessageBox(QMessageBox::Question, tr("Save"), tr("Saved successfully：")+ fileName+tr("-")+str, QMessageBox::Yes );
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
    for(int i=0;i<110;i++)
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

    inputDevSound = audioInputSound->start();
    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));

}

QAudioDeviceInfo MyThread::monitorVoiceSource(int i)
{
    qDebug()<<i;
    if(i==2)//系统内部
    {
        //当录制系统内部声音时吊用此方法
        QStringList deviceList;
        for( QAudioDeviceInfo &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
        {
            deviceList += deviceInfo.deviceName();
            if(deviceInfo.deviceName().contains("alsa_output"))
            {
                audioInputSound = new QAudioInput(deviceInfo, format, this);
                qDebug()<<"您选择了录制系统内部设备:"<<deviceInfo.deviceName()<<format;//可以检测系统内部声音
                return deviceInfo;
            }
            else
            {
                //qDebug()<<deviceInfo.deviceName();
            }

        }

    }
    else//其他
    {
        QAudioDeviceInfo inputDevice(QAudioDeviceInfo::defaultInputDevice());
        mFormatSound = inputDevice.nearestFormat(mFormatSound);
        audioInputSound = new QAudioInput(inputDevice, mFormatSound, this);
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
qDebug()<<"==========="<<MaxValue;
        MainWindow::mutual->valueArray[count] = MaxValue;//2020.11.12暂时禁用循环存值
        count++;//2020.11.12暂时禁用循环存值

        count=count%110;//2020.11.12暂时禁用循环存值
        emit recordPaint(MaxValue);//每检测到value就发送一次绘图信号
}
int MyThread::useVolumeSample(short sample)
{
    return qMax(qMin(((sample * soundVolume) / 100) ,30000),-30000);//求每次检测到的振幅组中最大值
}
void MyThread::listItemAdd(QString fileName)
{
    QStringList keyList = recordData->keys();
    if (keyList.contains("recorderpath"))
    {
        int  m=readNumList()-1;
        //qDebug()<<m;
        QStringList listRecordPath=readPathCollected().split(",");
        //qDebug()<<listRecordPath;
        for(int i=1;i<=m;i++)
        {
            QString str=listRecordPath.at(i);
            //qDebug()<<str;
            QFileInfo fileinfo(str);
            QString filesuffix = fileinfo.suffix();//判断文件后缀
//          qDebug()<<fileinfo.isFile();//判断是否为文件，是文件就存在了,因为在本地删除后，同步文件列表下才打开时那个文件也没了
            //qDebug()<<filesuffix;
            if(!str.contains(fileName)||m==1)
            {
                if(fileinfo.isFile()&&(filesuffix.contains("wav")||filesuffix.contains("mp3")||filesuffix.contains("m4a")))
                {
                    emit listItemAddSignal(fileName,MainWindow::mutual->list->count()+1);
                    break;//因为点击保存一次就添加一次所以break
                }
                else
                {
                    qDebug()<<str<<"文件或被删除！";
                    QString subStr = ","+str;//子串
                    /*
                     * 若文件路径已经消失,但配置文件里存在此路径。要更新配置文件中的路径字符串内容
                    */
                    QString oldStr = recordData->get("recorderpath").toString();
                    int pos = oldStr.indexOf(subStr);
                    //qDebug()<<pos<<" "<<oldStr;
                    //qDebug()<<oldStr.mid(pos,str.length()+1);
                    QString newStr = oldStr.remove(pos,str.length()+1);
                    writePathCollected(newStr);
                    writeNumList(readNumList()-1);
                    qDebug()<<readPathCollected();
                }

            }
            else
            {
                qDebug()<<"文件名相同！直接覆盖";
            }
        }
    }

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
            onChangeCurrentRecordList(fileName+tr(".mp3"));
            listItemAdd(fileName+tr(".mp3"));
            QMessageBox::information(NULL, tr("Save"), tr("Saved successfully:") + fileName);
        }

    }
    QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
    code->fromUnicode(fileName).data();
    if (fileName.isEmpty())
    {
        return ;
    }
    else
    {

        fileName += ".mp3";
        qDebug() << "filenamePath=" << fileName;

    }



}
void MyThread::selectM4a()
{
    if(fileName.length() == 0) {
        QMessageBox::information(NULL, tr("filename"), tr("You didn't select any files."));

    } else {
        if( toConvertM4a( "record.raw", (fileName+tr(".m4a")).toLocal8Bit().data() ) > 0 )
        {
            onChangeCurrentRecordList(fileName+tr(".m4a"));
            listItemAdd(fileName+tr(".m4a"));
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
            onChangeCurrentRecordList(fileName+tr(".wav"));
            listItemAdd(fileName+tr(".wav"));
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

