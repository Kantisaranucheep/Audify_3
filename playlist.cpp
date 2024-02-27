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
            QMessageBox::warning(nullptr, "Song Already Exists", "The selected song is already in the playlist.");
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

bool Playlist::operator==(const Playlist& other) const {
    // Implement the logic to compare two Playlist objects for equality
    return name == other.getName(); // Example: Comparing based on playlist name
}

void Playlist::setThumbnailPath(const QString& path)
{
    thumbnailPath = path;
}

QString Playlist::getThumbnailPath() const
{
    return thumbnailPath;
}
