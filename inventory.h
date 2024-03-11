#ifndef INVENTORY_H
#define INVENTORY_H

#include <QList>
#include "playlist.h"

class Inventory
{
public:
    void addPlaylist(const Playlist& playlist);

    void removePlaylist(int playlistindex);

    QList<Playlist> getPlaylists() const;

    bool playlistExists(const QString& playlistName) const;

    void clearPlaylists();

    QList<QString> searchPlaylists(const QString& searchText) const;
    const Playlist* findPlaylist(const QString& playlistName) const;
    void renamePlaylist(const QString& oldName, const QString& newName);

private:
    QList<Playlist> playlists;
};
#endif // INVENTORY_H
