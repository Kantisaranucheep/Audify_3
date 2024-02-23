//playlist.cpp
#include "playlist.h"

Playlist::Playlist(const QString& name)
    : name(name)
{
}

QString Playlist::getName() const
{
    return name;
}

void Playlist::importSong(const Song& song)
{
    for (const Song& existingSong : songs)
    {
        if (existingSong == song)
        {
            return;
        }
    }
    songs.append(song);
}

void Playlist::removeSong(int index)
{
    if (index >= 0 && index < songs.size())
    {
        songs.removeAt(index);
    }
}

QList<Song> Playlist::getSongs() const
{
    return songs;
}

int Playlist::getSongCount() const
{
    return songs.size();
}
