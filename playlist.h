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
    void setName(const QString& newName);

    void importSong(const Song& song);
    void removeSong(int index);
    QList<Song> getSongs() const;
    int getSongCount() const;

    qint64 getTotalDuration() const;
    bool operator==(const Playlist& other) const;

    void setThumbnailPath(const QString& path);
    QString getThumbnailPath() const;

    void clearSongs();

    QList<Song> searchSongs(const QString& searchText) const;
    const Song* findSongByName(const QString& songName) const;

private:
    QString name;
    QList<Song> songs;
    QString thumbnailPath;

};

#endif // PLAYLIST_H
