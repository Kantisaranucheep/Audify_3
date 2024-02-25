#include "song.h"




Song::Song(const QString& filename)
    : filename(filename)
{
}

QString Song::getfilename() const
{
    return filename;
}

bool Song::operator==(const Song& other) const {
    return filename == other.filename;
}

qint64 Song::getDuration() const {
    return duration;
}
