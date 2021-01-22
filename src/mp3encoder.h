/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  Authors: baijincheng <baijincheng@kylinos.cn>
 */
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
