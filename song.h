#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
public:
    mutable int playCount;

    Song(const QString& filename);
    QString getfilename() const;
    bool operator==(const Song& other) const;

    qint64 getDuration() const;
    // void setDuration(qint64 newDuration);
    int getPlayCount()const ;
    void incrementPlayCount();
    void setPlayCount(int count);


private:
    QString filename;
    qint64 duration;
    // mutable int playCount;
};

#endif // SONG_H

