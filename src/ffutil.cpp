#include "ffutil.h"

#include <QDebug>
#include <QImage>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavformat/avformat.h> //封装格式
#include <libavcodec/avcodec.h>   //解码
#include <libswscale/swscale.h>   //缩放
#include <libavdevice/avdevice.h> //设备
#include <libswresample/swresample.h>//???
#ifdef __cplusplus
}
#endif

#define LOG_TAG "ffutil"

FFUtil::FFUtil(QObject *parent) : QObject(parent)
{
    av_register_all();
    avdevice_register_all();//注册设备
}

int FFUtil::getDuration(QString path)
{
    int64_t duration = 0;

    av_register_all();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    avformat_open_input(&pFormatCtx, path.toStdString().c_str(), NULL, NULL);

    if (pFormatCtx) {
        avformat_find_stream_info(pFormatCtx, NULL);
        duration = pFormatCtx->duration / 1000;
    }

    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    return duration;
}
int FFUtil::open(QString _file)
{
    if(_file.length() == 0)
        return -1;
    if(pFormatCtx)
        avformat_close_input(&pFormatCtx);
    if(pCodecCtx)
        avcodec_free_context(&pCodecCtx);

    audioDuration   = 0;
    audioStream     = -1;
    pFormatCtx      = nullptr;
    pCodecCtx       = nullptr;
    AVDictionary *opts = nullptr;
    int t_seekTime = 0;
    if (avformat_open_input(&pFormatCtx, _file.toStdString().c_str(), 0, &opts) != 0)
    {
//        log_e("Can't open the file.");
        printf("can't open the file.");
        return -1;
    }

    // 找流信息
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
    {
//        log_e("Could't find stream infomation.");
        printf("Could't find stream infomation.");
        return -1;
    }

    audioStream = -1;
    audioStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioStream < 0)
    {
//        log_e("Can't find a audio stream.");
        printf("Can't find a audio stream.");
        return -1;
    }

    pCodec = avcodec_find_decoder(pFormatCtx->streams[audioStream]->codecpar->codec_id);
    if(!pCodec)
    {
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(!pCodecCtx)
    {
//        log_e("Codec context alloc error.");
        qDebug() << "codec context alloc error!";
        return -1;
    }

    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[audioStream]->codecpar);

    int re = avcodec_open2(pCodecCtx, pCodec, 0);
    if (re != 0)
    {
//        log_e("Open codec error.");
        avcodec_free_context(&pCodecCtx);
        return -1;
    }
    return 0;
}
