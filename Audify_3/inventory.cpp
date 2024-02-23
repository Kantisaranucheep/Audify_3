#include "inventory.h"

void Inventory::addPlaylist(const Playlist& playlist)
{
    playlists.append(playlist);
}

QList<Playlist> Inventory::getPlaylists() const
{
    return playlists;
}

void Inventory::removePlaylist(const QString& playlistName)
{
    // Find and remove the playlist with the specified name
    auto it = std::remove_if(playlists.begin(), playlists.end(),
                             [playlistName](const Playlist& playlist)
                             {
                                 return playlist.getName() == playlistName;
                             });

    // Erase the removed elements from the list
    playlists.erase(it, playlists.end());
}
