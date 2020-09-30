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
    pTimer = new QTimer;
    audioInputSound = nullptr;
    audioOutputSound = nullptr;

    inputDevSound = nullptr;

    my_time=new QTimer;//延时检测
    connect(my_time,&QTimer::timeout,this,[=]{
        canMonitor=true;
    });
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

    WavFileHeader.BitsPerSample = 16;
    WavFileHeader.BytesPerSample = 2;
    WavFileHeader.SampleRate = 8000;
    WavFileHeader.BytesPerSecond = 16000;
    WavFileHeader.ChannleNumber = 1;
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

    WavFileHeader.RiffLength = nFileLen - 8 + nSize;
    WavFileHeader.DataLength = nFileLen;

    // 先将wav文件头信息写入，再将音频数据写入;
    wavFile.write((char *)&WavFileHeader, nSize);
    wavFile.write(cacheFile.readAll());

    cacheFile.close();
    wavFile.close();
    return nFileLen;
}


void MyThread::record_pressed()
{
    qDebug()<<"开始录音:";


    my_time->start(120);//每隔xms检测一次，波形图缓慢刷新
    InitMonitor();
    file =new QFile();
    file->setFileName(tr("record.raw"));
    bool is_open =file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(!is_open)
    {
        qDebug()<<"打开失败";
        exit(1);
    }

    QAudioFormat format;
    format.setSampleRate(8000);/*每秒钟对声音的采样次数，越大越精细，44100HZ(标准CD级)，
                                8000HZ的采样率(每秒钟采集8000个声波的点)*/
    format.setChannelCount(1);//平声道数目为1；立体声数目为2
    format.setSampleSize(16);
    format.setCodec("audio/pcm");//编码器
    format.setByteOrder(QAudioFormat::LittleEndian);//低位优先
    format.setSampleType(QAudioFormat::SignedInt);//平声道（8bit），QAudioFormat::UnSignedInt已足够。

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();//获取设备信息
    QString str=info.deviceName();//获取设备名
    qDebug()<<"使用的录音设备是:"<<str;
    if(!info.isFormatSupported(format))
    {
        format = info.nearestFormat(format);
    }
    audioInputFile = new QAudioInput(format, this);
    qDebug()<<"录音开始";
    audioInputFile->start(file);

}
void MyThread::stop_btnPressed()//停止录音并弹出存储为。。。
{

    audioInputFile->stop();//音频输入停止
    audioInputSound->stop();//监听停止
    file->close();

qDebug()<<"成功停止！";
    QSettings my_ini;
    int ad =my_ini.value("save_default").toInt();
    int seq=my_ini.value("sequence_default").toInt();//默认是0
//    seq=-1;
    //qDebug()<<ad;
    if(ad==1)
    {
        //弹存储为的框
        saveas.show();
    }
    else
    {
        qDebug()<<seq;
        QString str = QString::number(seq);
        desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString Date=QDate::currentDate().toString(Qt::ISODate);
        fileName=Date;
//        qDebug()<<fileName;
        if( addWavHeader( "record.raw", (desktop_path+tr("/")+fileName+tr("-")+str+tr(".wav")).toLocal8Bit().data() ) > 0 )
        {
            //WrrMsg = new QMessageBox(QMessageBox::NoIcon,tr("保存"), tr("保存成功 :") + fileName+tr("-")+str, QMessageBox::Ok, 0);
            WrrMsg = new QMessageBox(QMessageBox::Question, tr("保存"), tr("保存成功：")+ fileName+tr("-")+str, QMessageBox::Yes );
            WrrMsg->button(QMessageBox::Yes)->setText("确 定");
            WrrMsg->exec();
        }

        QTextCodec *code=QTextCodec::codecForName("gb2312");//解决中文路径保存
        code->fromUnicode(fileName).data();

    }
    seq++;
    qDebug()<<seq;
    my_ini.setValue("sequence_default",seq);
    delete audioInputFile;
    audioInputFile = nullptr;
    delete audioInputSound;
    audioInputSound =nullptr;
    delete file;
    file = nullptr;
}
void MyThread::pauseRecord()//暂停录制
{
    audioInputFile->stop();
    audioInputSound->stop();
}
void MyThread::playRecord()//开始录制
{

    audioInputFile->start(file);

    outputDevSound = audioOutputSound->start();
    inputDevSound = audioInputSound->start();
    //qDebug()<<mpAudioInputSound->volume();
    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));
}
void MyThread::InitMonitor()
{

    //判断当前的音频输入设备是否支持QAudioFormat配置，如果不支持，获取支持的最接近的配置信息
    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(mFormatSound))
    {
        //默认不支持时找最近设备
        mFormatSound = infoIn.nearestFormat(mFormatSound);
        qDebug()<<"输入！！！";
    }
    //CreateAudioInput();
    //************************************************2020.9.24
    inputDevSound=nullptr;
    audioInputSound=nullptr;
    audioOutputSound=nullptr;
    if (inputDevSound != 0) {
        disconnect(inputDevSound, 0, this, 0);
        inputDevSound = 0;

    }

    QAudioDeviceInfo inputDevice(QAudioDeviceInfo::defaultInputDevice());
    audioInputSound = new QAudioInput(inputDevice, mFormatSound, this);

//    CreateAudioOutput();
    QAudioDeviceInfo outputDevice(QAudioDeviceInfo::defaultOutputDevice());
    audioOutputSound = new QAudioOutput(outputDevice, mFormatSound, this);


    outputDevSound = audioOutputSound->start();
    inputDevSound = audioInputSound->start();

//    qDebug()<<"mpAudioInputSound->volume()";

    //mpOutputDevSound的信号槽
    connect(inputDevSound, SIGNAL(readyRead()),this,SLOT(OnReadMore()));

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
//        if(false)
//        {
//            qDebug()<<"进行去噪处理";
//            //Remove noise using Low Pass filter algortm[Simple algorithm used to remove noise]
//            for ( iIndex=1; iIndex < len; iIndex++ ) {
//                outdata[ iIndex ] = 0.333 * resultingData[iIndex ] + ( 1.0 - 0.333 ) * outdata[ iIndex-1 ];
//            }
//        }
        MaxValue = 0;
        for (int i=0; i<len;i++ )
        {
            //把样本数据转换为整型
            value = abs(useVolumeSample(outdata[i]));/*麦克风中的薄膜始终是在平衡位置附近
                                                  value会检测到正负相间的震荡频率,加个绝对值*/
            MaxValue = MaxValue>=value ? MaxValue : value;
           //if(0==miMaxValue)miMaxValue=5;

        }
        emit recordPaint(MaxValue);
}
int MyThread::useVolumeSample(short sample)
{
    return qMax(qMin(((sample * soundVolume) / 50) ,30000),-30000);//求每次检测到的振幅组中最大值
}
void MyThread::select()
{
    fileName = QFileDialog::getSaveFileName(
                      this,
                      tr("cstopButtohoose a file to save under"),
                          QDir::currentPath(),
                          "Wav(*.wav)");
    if(fileName.length() == 0) {
        QMessageBox::information(NULL, tr("filename"), tr("You didn't select any files."));
    } else {
        if( addWavHeader( "record.raw", (fileName+tr(".wav")).toLocal8Bit().data() ) > 0 )
            qDebug() << "1";
        QMessageBox::information(NULL, tr("Save"), tr("Success Save :") + fileName);
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
