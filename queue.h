#ifndef QUEUE_H
#define QUEUE_H

#include <QString>
#include <QList>
#include "song.h"

class queue
{
public:
    queue(const QString& name);
    QString getName() const;

    void importSong(const Song& song);
    QList<Song> getSongs() const;

private:
    QString name;
    QList<Song> songs;
};

#endif // QUEUE_H
