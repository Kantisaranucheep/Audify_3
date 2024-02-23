#include "queue.h"

queue::queue(const QString& name) : name(name) {}

QString queue::getName() const {
    return name;
}

void queue::importSong(const Song& song) {
    songs.append(song);
}

QList<Song> queue::getSongs() const {
    return songs;
}
