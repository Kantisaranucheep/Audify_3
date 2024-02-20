#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
public:
    Song(const QString& filename);
    QString getfilename() const;

private:
    QString filename;
};

#endif // SONG_H

