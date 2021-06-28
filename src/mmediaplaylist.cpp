#include "mmediaplaylist.h"

MMediaPlaylist::MMediaPlaylist(QObject *parent)
    : QObject(parent)
{

}

QString MMediaPlaylist::getPlayFileName()
{
    if (m_playerList.isEmpty()) {
        qDebug()<<"播放列表为空";
        return "";
    }
    if (m_index >= m_playerList.length()) {
        m_index = m_playerList.length();
        return m_playerList.last().toString();
    }
    return m_playerList.at(m_index).toString();
}


int MMediaPlaylist::currentIndex() const
{
    return m_index;
}

bool MMediaPlaylist::addMedia(const QUrl &items)
{
    m_playerList.append(items);
    return true;
}

void MMediaPlaylist::next()
{
    if (m_playerList.isEmpty()) {
        return;
    }
    switch (m_playbackMode) {
    case Random:
        m_index=randomIndex();
        break;
    case Sequential:
        m_index++;
        if (m_index >= m_playerList.length()) {
           m_index = m_playerList.length() - 1;
        }
        break;
    default:
        m_index++;
        if (m_index >= m_playerList.length()) {
           m_index = 0;
        }
        break;
    }
    emit currentIndexChanged(m_index);
    emit stop();
}

void MMediaPlaylist::previous()
{
    if (m_playerList.isEmpty()) {
        return;
    }

    switch (m_playbackMode) {
    case Random:
        m_index=randomIndex();
        break;
    case Sequential:
        m_index--;
        if (m_index < 0) {
            m_index = 0;
        }
        break;
    default:
        m_index--;
        if (m_index < 0) {
           m_index = m_playerList.length() - 1;
        }
        break;
    }
    emit currentIndexChanged(m_index);
    emit stop();
}

void MMediaPlaylist::setCurrentIndex(int index)
{
    if (index >= m_playerList.length()) {
        qDebug()<<"指定位置超过列表元素数量";
        return;
    }
    m_index = index;
    emit currentIndexChanged(m_index);
}

void MMediaPlaylist::setPlaybackMode(MMediaPlaylist::PlaybackMode mode)
{
    m_playbackMode = mode;
}

int MMediaPlaylist::mediaCount() const
{
    return m_playerList.length();
}

MMediaContent MMediaPlaylist::media(int index) const
{
    if (index >= m_playerList.length()) {
        return MMediaContent(QUrl(""));
    }
    return MMediaContent(m_playerList.at(index));
}

bool MMediaPlaylist::clear()
{
    m_playerList.clear();
    return true;
}

bool MMediaPlaylist::removeMedia(int pos)
{
    if (pos >= m_playerList.length()) {
        return false;
    }
    m_playerList.removeAt(pos);
    return true;
}

void MMediaPlaylist::palyFinish()
{
    if (m_playbackMode != CurrentItemInLoop) {
        next();
    }
    emit autoPlay(m_playbackMode);
}

MMediaPlaylist::PlaybackMode MMediaPlaylist::playbackMode() const
{
    return m_playbackMode;
}

int MMediaPlaylist::randomIndex()
{
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
    return qrand()%(m_playerList.length());
}


MMediaContent::MMediaContent(QUrl url)
{
    m_url = url;
}

QUrl MMediaContent::canonicalUrl() const
{
    return m_url;
}
