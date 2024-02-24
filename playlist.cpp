#include "playlist.h"
#include <QMessageBox>

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
            QMessageBox::information(nullptr, "Song Already Exists", "The selected song is already in the playlist.");
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

qint64 Playlist::getTotalDuration() const {
    qint64 totalDuration = 0;
    for (const Song& song : songs) {
        totalDuration += song.getDuration();
    }
    return totalDuration;
}
