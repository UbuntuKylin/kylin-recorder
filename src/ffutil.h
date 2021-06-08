#ifndef FFUTIL_H
#define FFUTIL_H

#include <QObject>

struct AVCodec;
struct AVCodecContext;
struct AVFormatContext;
struct AVOutputFormat;

class FFUtil : public QObject
{
    Q_OBJECT
public:

    enum AudioFormatType
        {
            AudioFormat_WAV,
            AudioFormat_MP3,
            AudioFormat_M4A
        };

    explicit FFUtil(QObject *parent = nullptr);
    int open(QString _file);
    int getDuration();
    void close();

    void generateAudioFile(AudioFormatType formatType, QString fileName, QByteArray pcmData);

private:

    AVOutputFormat  *pOutputFormat = nullptr;
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext  *pCodecCtx = nullptr;
    AVCodec         *pCodec = nullptr;
    int audioStream;
    int audioDuration;

    AVCodec *m_AudioCodec = nullptr;
    AVCodecContext *m_AudioCodecContext = nullptr;
    AVFormatContext *m_FormatContext = nullptr;
    AVOutputFormat *m_OutputFormat = nullptr;

    // init Format
    bool initFormat(QString audioFileName);

signals:

};

#endif // FFUTIL_H
