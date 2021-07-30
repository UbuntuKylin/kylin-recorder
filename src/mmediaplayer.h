#ifndef MMEDIAPLAYER_H
#define MMEDIAPLAYER_H

#include <QObject>
#include <QUrl>
#include <QDebug>
#include <client.h> // MPV库头文件

#include "mmediaplaylist.h"

class MMediaPlayer : public QObject
{
    Q_OBJECT
public:
    enum State{StoppedState=0,PlayingState,PausedState};
    MMediaPlayer(QObject *parent = nullptr);

    void setPlaylist(MMediaPlaylist *playlist);
    void pause();

    State state() const;
    qint64 position() const;
    void setPosition(qint64 pos);

    bool isAvailable() const;
    void setVolume(int vol);
    qint64 duration() const;
    void setMedia(const MMediaContent &media);
    void play();
public slots:
    void stop();
    void onMpvEvents(); //接收mpv事件

private:
    void truePlay(QString startTime = "0");
    void handle_mpv_event(mpv_event *event); // 处理mpv事件
    void createMvpplayer(); // 创建mvpPlayer
    void setProperty(const QString &name, const QString &value); // 设置mpv属性
    QString getProperty(const QString &name) const; // 获得mpv属性
    void changeState(State stateNow);
    MMediaPlaylist * m_playList = nullptr;
    MMediaPlaylist * m_tmpPlayList = nullptr;
    mpv_handle *m_mpvPlayer = nullptr;//句柄
    State m_state = StoppedState;//播放状态
    QByteArray filenameBack = "";
    bool m_positionChangeed = false;
    qint64 m_position = 0;
    qint64 m_duration = 0;
private slots:
    void autoPlay(MMediaPlaylist::PlaybackMode playbackMode);
signals:
    void mpvEvents(); // 触发onMpvEvents()槽函数的信号
    void stateChanged(MMediaPlayer::State);
    void durationChanged(qint64);
    void positionChanged(qint64);
    void playFinish();
};

#endif // MMEDIAPLAYER_H
