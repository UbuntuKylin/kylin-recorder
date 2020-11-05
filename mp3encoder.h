#ifndef MP3ENCODER_H
#define MP3ENCODER_H

#include <QObject>
#include <lame/lame.h>//记得安装lame库
class Mp3Encoder : public QObject
{
    Q_OBJECT
public:
    explicit Mp3Encoder(int rate, int channels,QObject *parent = nullptr);
    ~Mp3Encoder();  //退出编码的工作
    int encode(short *pcm_data,  int pcmdata_len, char *mp3_data, int mp3data_len); //编码
    int getLastMp3Data(char *mp3_data, int mp3data_len);  //获取最后的mp3数据
private:
    lame_global_flags *gfp; //存放配置信息
    int channels;
signals:

};

#endif // MP3ENCODER_H
