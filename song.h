#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
public:
    Song(const QString& filename);
    QString getfilename() const;
    bool operator==(const Song& other) const;

    qint64 getDuration() const;
    // void setDuration(qint64 newDuration);


private:
    QString filename;
    qint64 duration;
};

#endif // SONG_H

