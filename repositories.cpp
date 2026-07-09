#include "repositories.h"

Song* SongRepository::save(Song* entity) {
    if (entity->getId() == 0) {
        QList<Song*> existing = storage;
        for (int i = 0; i < existing.size(); ++i) {
            if (existing.at(i)->getName() == entity->getName()) {
                delete entity;
                return existing.at(i);
            }
        }
        Song* newSong = new Song(nextId++, entity->getName(), 0, "", "", entity->getArtistId(), entity->getAlbumId());
        storage.append(newSong);
        delete entity;
        return newSong;
    }
    return entity;
}

bool SongRepository::remove(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) {
            delete storage.takeAt(i);
            return true;
        }
    }
    return false;
}

Song* SongRepository::search(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) return storage.at(i);
    }
    return nullptr;
}

QList<Song*> SongRepository::singleSongs(int artistId) {
    QList<Song*> result;
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getArtistId() == artistId && storage.at(i)->getAlbumId() == 0) {
            result.append(storage.at(i));
        }
    }
    return result;
}

QList<Song*> SongRepository::getByAlbum(int albumId) {
    QList<Song*> result;
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getAlbumId() == albumId) result.append(storage.at(i));
    }
    return result;
}

QList<Song*> SongRepository::getByArtist(int artistId) {
    QList<Song*> result;
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getArtistId() == artistId) result.append(storage.at(i));
    }
    return result;
}

Playlist* PlaylistRepository::save(Playlist* entity) {
    storage.append(entity);
    return entity;
}

bool PlaylistRepository::remove(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) {
            delete storage.takeAt(i);
            return true;
        }
    }
    return false;
}

Playlist* PlaylistRepository::search(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) return storage.at(i);
    }
    return nullptr;
}

void PlaylistRepository::insertSong(int playlistId, int songId) {
    Playlist* pl = search(playlistId);
    if (pl) pl->addSong(songId);
}

void PlaylistRepository::removeSong(int playlistId, int songId) {
    Playlist* pl = search(playlistId);
    if (pl) pl->removeSong(songId);
}

QList<Playlist*> PlaylistRepository::playlists(int listenerId) {
    QList<Playlist*> result;
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getListenerId() == listenerId) result.append(storage.at(i));
    }
    return result;
}

Account* ArtistRepository::save(Account* entity) {
    storage.append(dynamic_cast<Artist*>(entity));
    return entity;
}

bool ArtistRepository::remove(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) {
            delete storage.takeAt(i);
            return true;
        }
    }
    return false;
}

Account* ArtistRepository::search(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) return storage.at(i);
    }
    return nullptr;
}

QList<Account*> ArtistRepository::getAll() {
    QList<Account*> result;
    for (int i = 0; i < storage.size(); ++i) result.append(storage.at(i));
    return result;
}

Account* ArtistRepository::searchByUserName(QString userName) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getUserName() == userName) return storage.at(i);
    }
    return nullptr;
}

Account* ListenerRepository::save(Account* entity) {
    storage.append(dynamic_cast<Listener*>(entity));
    return entity;
}

bool ListenerRepository::remove(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) {
            delete storage.takeAt(i);
            return true;
        }
    }
    return false;
}

Account* ListenerRepository::search(int id) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getId() == id) return storage.at(i);
    }
    return nullptr;
}

QList<Account*> ListenerRepository::getAll() {
    QList<Account*> result;
    for (int i = 0; i < storage.size(); ++i) result.append(storage.at(i));
    return result;
}

Account* ListenerRepository::searchByUserName(QString userName) {
    for (int i = 0; i < storage.size(); ++i) {
        if (storage.at(i)->getUserName() == userName) return storage.at(i);
    }
    return nullptr;
}

void ListenerRepository::updateLiked(int listenerId, int songId, bool isLiked) {}

bool ListenerRepository::isLiked(int listenerId, int songId) {
    return false;
}