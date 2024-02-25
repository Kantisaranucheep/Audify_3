#include "inventory.h"
#include <QMessageBox>

void Inventory::addPlaylist(const Playlist& playlist)
{
    // Check if a playlist with the same name already exists
    for(const Playlist& existingPlaylist : playlists){
        if(existingPlaylist == playlist){
            // Playlist with the same name already exists, show a message box
            QMessageBox::warning(nullptr, "Duplicate Playlist Name",
                                 "A playlist with the same name already exists. Please choose a different name.");
            return;
        }
    }

    // No playlist with the same name, add the new playlist
    playlists.append(playlist);
}
QList<Playlist> Inventory::getPlaylists() const
{
    return playlists;
}

void Inventory::removePlaylist(int playlistIndex)
{
    // Check if the index is within the valid range
    if (playlistIndex >= 0 && playlistIndex < playlists.size()) {
        // Erase the playlist at the specified index
        playlists.removeAt(playlistIndex);
    }
}

bool Inventory::playlistExists(const QString& playlistName) const {
    for (const Playlist& playlist : playlists) {
        if (playlist.getName() == playlistName) {
            return true; // Playlist with the same name already exists
        }
    }
    return false; // Playlist with the given name does not exist
}
