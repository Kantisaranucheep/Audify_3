#include "song.h"

Song::Song(const QString& filename)
    : filename(filename), duration(0), playCount(0)
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

// void Song::setDuration(qint64 newDuration) {
//     duration = newDuration;
// }

int Song::getPlayCount()const{
    return playCount;
}

void Song::incrementPlayCount(){
    ++playCount;
}

void Song::setPlayCount(int count)
{
    playCount = count;
}
