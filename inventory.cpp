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
        // Check if trying to remove the default playlist
        if (playlistIndex == 0) {
            // QMessageBox::information(nullptr, "Cannot Delete Default Playlist",
            //                          "The default playlist 'All Songs' cannot be deleted.");
            return;
        } else {
            // Erase the playlist at the specified index
            playlists.removeAt(playlistIndex);
        }
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

void Inventory::clearPlaylists() {
    playlists.clear();
}

QList<QString> Inventory::searchPlaylists(const QString& searchText) const
{
    QList<QString> searchResults;

    for (const Playlist& playlist : playlists) {
        if (playlist.getName().contains(searchText, Qt::CaseInsensitive)) {
            searchResults.append(playlist.getName());
        }
    }

    return searchResults;
}

const Playlist* Inventory::findPlaylist(const QString& playlistName) const {
    for (const Playlist& playlist : playlists) {
        if (playlist.getName() == playlistName) {
            return &playlist;
        }
    }
    return nullptr;
}

void Inventory::renamePlaylist(const QString& oldName, const QString& newName)
{
    // Find the playlist with the old name
    auto it = std::find_if(playlists.begin(), playlists.end(), [&oldName](const Playlist& playlist) {
        return playlist.getName() == oldName;
    });

    // If found, rename the playlist
    if (it != playlists.end()) {
        it->setName(newName);
    }
    // You might want to handle the case where the playlist with the old name is not found.
}
