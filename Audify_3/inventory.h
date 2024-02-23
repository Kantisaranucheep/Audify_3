#ifndef INVENTORY_H
#define INVENTORY_H

#include <QList>
#include "playlist.h"

class Inventory
{
public:
    void addPlaylist(const Playlist& playlist);

    void removePlaylist(const QString& playlistName);

    QList<Playlist> getPlaylists() const;

private:
    QList<Playlist> playlists;
};
#endif // INVENTORY_H
