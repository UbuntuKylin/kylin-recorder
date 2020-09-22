#include "recording.h"
#include "mainwindow.h"
#include "mywave.h"

#define BufferSize         35280
#define rectangleCount 40//矩形条个数
Recording::Recording(QObject *parent) : QObject(parent)
{


}

void Recording::test()
{
    while(1)
    {
        qDebug()<<"子线程ID:"<<QThread::currentThread()<<i;
        QThread::sleep(1);
        i++;
    }


}
void Recording::record_press()
{
    file =new QFile();
    file->setFileName(tr("record.raw"));
    bool is_open =file->open(QIODevice::WriteOnly | QIODevice::Truncate);
    if(!is_open)
    {
        qDebug()<<"打开失败"<<endl;
//           CreateWavFile(RAW_RECORD_FILENAME,WAV_RECORD_FILENAME);
        exit(1);
    }
    else
    {
        qDebug()<<"打开成功"<<endl;
    }

    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    QString str=info.deviceName();
    qDebug()<<"使用的录音设备是:"<<str<<endl;
    if(!info.isFormatSupported(format))
    {
        format = info.nearestFormat(format);
    }
    m_audioInput = new QAudioInput(format, this);
    qDebug()<<"录音开始";
    m_audioInput->start(file);

//    stopButton->setIcon(QIcon(":/png/png/stop_click.png"));
//    play_pauseButton->setEnabled(true);//按下后，开始录音可以按暂停
//    play_pauseButton->setIcon(QIcon(":/png/png/pause.png"));
    baseTime = this->baseTime.currentTime();

    qDebug()<<baseTime.currentTime().toString();//输出当前时间
    pTimer->start();
//    stop=true;
}

void Recording::OnReadMore()
{
//    if(!canMonior)return;//
//    canMonior=false;
    //Return if audio input is null
    if(!mpAudioInputSound)
    {
        qDebug()<<" REISIVE VOICE";
        return;
    }
    //qDebug()<<"检测到声音";
    QByteArray _Buffer(BufferSize, 0);
    //Check the number of samples in input buffer
    len = mpAudioInputSound->bytesReady();

    //Limit sample size
    if(len > 4096)
        len = 4096;
//qDebug()<<"！";
    //Read sound samples from input device to buffer
    qint64 l = mpInputDevSound->read(_Buffer.data(), len);
    if(l <= 0) return;//

        //qDebug()<<l;
        //Assign sound samples to short array
        short* resultingData = (short*)_Buffer.data();

        outdata=resultingData;
        outdata[ 0 ] = resultingData [ 0 ];

        int iIndex;
        if(false)
        {
            qDebug()<<"进行去噪处理";
            //Remove noise using Low Pass filter algortm[Simple algorithm used to remove noise]
            for ( iIndex=1; iIndex < len; iIndex++ ) {
                outdata[ iIndex ] = 0.333 * resultingData[iIndex ] + ( 1.0 - 0.333 ) * outdata[ iIndex-1 ];
            }
        }
        miMaxValue = 0;
        for ( iIndex=0; iIndex < len; iIndex++ )
        {
            //Cange volume to each integer data in a sample
            value = ApplyVolumeToSample(outdata[ iIndex ]);
            miMaxValue = miMaxValue>=abs(value) ? miMaxValue : abs(value);
//            if(0==miMaxValue)miMaxValue=5;
        }
        qDebug()<<"***************"<<miMaxValue;

        maxNum.prepend(miMaxValue/350);//将元素插入到Vector的开始
        maxNum=maxNum.mid(0,rectangleCount);
//emit setPaintNumList(maxNum);
        qDebug()<<maxNum;
        for(int i=0;i<rectangleCount;i++)
        {
            if(maxNum.at(i)>5&&maxNum.at(i)<=10)
            {
                mywave.at(i)->setValue(maxNum.at(i)*2);
            }
            else
            {
                mywave.at(i)->setValue(maxNum.at(i));
            }
        }

    //write modified sond sample to outputdevice for playback audio
    //pIODevice->write((char *)audio_out_buffer,out_buffer_size);
    //mpOutputDevSound->write((char*)outdata, len);//本行注释方可去掉回音


}
int Recording::ApplyVolumeToSample(short iSample)
{
//    qDebug()<<"！！";
    //Calculate volume, Volume limited to  max 30000 and min -30000
    return std::max(std::min(((iSample * miVolume) / 50) ,30000), -30000);//替换成QT的方法

}
void Recording::CreateAudioInput()
{

//    qDebug()<<"入！！！";
    if (mpInputDevSound != 0) {
        disconnect(mpInputDevSound, 0, this, 0);
        mpInputDevSound = 0;
    }

    QAudioDeviceInfo inputDevice(QAudioDeviceInfo::defaultInputDevice());
    mpAudioInputSound = new QAudioInput(inputDevice, mFormatSound, this);

}

void Recording::CreateAudioOutput()
{
    qDebug()<<"！！！";
    QAudioDeviceInfo outputDevice(QAudioDeviceInfo::defaultOutputDevice());
    mpAudioOutputSound = new QAudioOutput(outputDevice, mFormatSound, this);

}
