#include "song.h"

Song::Song(const QString& filename)
    : filename(filename)
{
}

QString Song::getfilename() const
{
    return filename;
}
