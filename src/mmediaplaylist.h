#ifndef MMediaPlaylist_H
#define MMediaPlaylist_H

#include <QObject>
#include <QDebug>
#include <QUrl>
#include <QDateTime>

class MMediaContent
{

public:
    MMediaContent(QUrl url);
    QUrl canonicalUrl() const;
private:
    QUrl m_url;
};


class MMediaPlaylist : public QObject
{
    Q_OBJECT
public:
    enum PlaybackMode { CurrentItemOnce=0, CurrentItemInLoop, Sequential, Loop, Random };
    MMediaPlaylist(QObject *parent = nullptr);

    QString getPlayFileName();
    int currentIndex() const;
    bool addMedia(const QUrl &items);
    void next();
    void previous();
    void setCurrentIndex(int index);
    PlaybackMode playbackMode() const;
    void setPlaybackMode(PlaybackMode mode);
    int mediaCount() const;
    MMediaContent media(int index) const;
    bool clear();
    bool removeMedia(int pos);
public slots:
    void palyFinish();
private:
    int randomIndex();
    QList<QUrl> m_playerList;
    int m_index=0;
    PlaybackMode m_playbackMode = Loop;
signals:
    void currentIndexChanged(int);
    void playbackModeChanged(PlaybackMode);
    void autoPlay(MMediaPlaylist::PlaybackMode playbackMode);
    void stop();
};

#endif // MMediaPlaylist_H
