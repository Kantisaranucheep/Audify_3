#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QList>
#include "song.h"

class Playlist
{
public:
    Playlist(const QString& name);
    QString getName() const;

    void importSong(const Song& song);
    void removeSong(int index);
    QList<Song> getSongs() const;

private:
    QString name;
    QList<Song> songs;
};

#endif // PLAYLIST_H
