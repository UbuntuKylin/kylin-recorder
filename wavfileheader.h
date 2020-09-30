#ifndef WAVFILEHEADER_H
#define WAVFILEHEADER_H

#include <QObject>

class WavFileHeader : public QObject
{
    Q_OBJECT
public:
    explicit WavFileHeader(QObject *parent = nullptr);

    char RiffName[4];// RIFF 头
    uint32_t nRiffLength;//长度
    char WavName[4];//数据类型标识符
    char FmtName[4];// 格式块中的头
    uint32_t nFmtLength;

    uint16_t nAudioFormat;
    uint16_t nChannleNumber;
    uint32_t nSampleRate;
    uint32_t nBytesPerSecond;
    uint16_t nBytesPerSample;
    uint16_t nBitsPerSample;


    char DATANAME[4];//  catheFileName , QString wavF数据块中的块头
    uint32_t nDataLength;
signals:

};

#endif // WAVFILEHEADER_H
