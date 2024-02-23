#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
public:
    Song(const QString& filename);
    QString getfilename() const;
    bool operator==(const Song& other) const;

private:
    QString filename;
};

#endif // SONG_H

