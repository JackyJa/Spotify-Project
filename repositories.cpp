#include "repositories.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<Song*> SongRepository::getAll() {
    QList<Song*> result;
    QSqlQuery query("SELECT * FROM songs");
    while (query.next()) {
        result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                               query.value(2).toInt(), query.value(3).toString(),
                               query.value(4).toString(), query.value(5).toInt(),
                               query.value(6).toInt(), query.value(7).toString()));
    }
    return result;
}

Song* SongRepository::save(Song* entity) {
    QSqlQuery query;
    query.prepare("INSERT INTO songs (name, releaseYear, genre, audioFilePath, artistId, albumId, coverPath) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(entity->getName());
    query.addBindValue(entity->getReleaseYear());
    query.addBindValue(entity->getGenre());
    query.addBindValue(entity->getAudioFilePath());
    query.addBindValue(entity->getArtistId());
    query.addBindValue(entity->getAlbumId());
    query.addBindValue(entity->getCoverPath());


    if (query.exec()) {
        delete entity;
        return nullptr;
    }
    return entity;
}

bool SongRepository::remove(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM songs WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

Song* SongRepository::search(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM songs WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return new Song(query.value(0).toInt(), query.value(1).toString(),
                        query.value(2).toInt(), query.value(3).toString(),
                        query.value(4).toString(), query.value(5).toInt(),
                        query.value(6).toInt(), query.value(7).toString());
    }
    return nullptr;
}

QList<Song*> SongRepository::singleSongs(int artistId) {
    QList<Song*> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM songs WHERE artistId = ? AND albumId = 0");
    query.addBindValue(artistId);
    if (query.exec()) {
        while (query.next()) {

            result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt(), query.value(3).toString(),
                                   query.value(4).toString(), query.value(5).toInt(),
                                   query.value(6).toInt(), query.value(7).toString()));
        }
    }
    return result;
}

QList<Song*> SongRepository::getByAlbum(int albumId) {
    QList<Song*> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM songs WHERE albumId = ?");
    query.addBindValue(albumId);
    if (query.exec()) {
        while (query.next()) {
            result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt(), query.value(3).toString(),
                                   query.value(4).toString(), query.value(5).toInt(),
                                   query.value(6).toInt(), query.value(7).toString()));
        }
    }
    return result;
}

QList<Song*> SongRepository::getByArtist(int artistId) {
    QList<Song*> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM songs WHERE artistId = ?");
    query.addBindValue(artistId);
    if (query.exec()) {
        while (query.next()) {

            result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt(), query.value(3).toString(),
                                   query.value(4).toString(), query.value(5).toInt(),
                                   query.value(6).toInt(), query.value(7).toString()));
        }
    }
    return result;
}

QList<Song*> SongRepository::getByPlaylist(int playlistId) {
    QList<Song*> result;
    QSqlQuery query;
    query.prepare("SELECT s.* FROM songs s JOIN playlist_songs ps ON s.id = ps.songId WHERE ps.playlistId = ?");
    query.addBindValue(playlistId);
    if (query.exec()) {
        while (query.next()) {
            result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt(), query.value(3).toString(),
                                   query.value(4).toString(), query.value(5).toInt(),
                                   query.value(6).toInt()));
        }
    }
    return result;
}

QList<Song*> SongRepository::searchSongs(QString name, QString genre, int year) {
    QList<Song*> result;
    QSqlQuery query;
    QString queryStr = "SELECT * FROM songs WHERE 1=1";

    if (!name.isEmpty()) queryStr += " AND name LIKE ?";
    if (!genre.isEmpty()) queryStr += " AND genre LIKE ?";
    if (year != 0) queryStr += " AND releaseYear = ?";

    query.prepare(queryStr);

    if (!name.isEmpty()) query.addBindValue("%" + name + "%");
    if (!genre.isEmpty()) query.addBindValue("%" + genre + "%");
    if (year != 0) query.addBindValue(year);

    if (query.exec()) {
        while (query.next()) {
            result.append(new Song(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt(), query.value(3).toString(),
                                   query.value(4).toString(), query.value(5).toInt(),
                                   query.value(6).toInt(), query.value(7).toString()));
        }
    }
    return result;
}

QList<Playlist*> PlaylistRepository::getAll() {
    QList<Playlist*> result;
    QSqlQuery query("SELECT * FROM playlists");
    while (query.next()) {
        result.append(new Playlist(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toInt()));
    }
    return result;
}

Playlist* PlaylistRepository::save(Playlist* entity) {
    QSqlQuery query;
    query.prepare("INSERT INTO playlists (name, listenerId) VALUES (?, ?)");
    query.addBindValue(entity->getName());
    query.addBindValue(entity->getListenerId());

    if (query.exec()) {
        delete entity;
        return nullptr;
    }
    return entity;
}

bool PlaylistRepository::remove(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM playlists WHERE id = ?");
    query.addBindValue(id);
    query.exec();

    query.prepare("DELETE FROM playlist_songs WHERE playlistId = ?");
    query.addBindValue(id);
    return query.exec();
}

Playlist* PlaylistRepository::search(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM playlists WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return new Playlist(query.value(0).toInt(), query.value(1).toString(),
                            query.value(2).toInt());
    }
    return nullptr;
}

void PlaylistRepository::insertSong(int playlistId, int songId) {
    QSqlQuery query;
    query.prepare("INSERT INTO playlist_songs (playlistId, songId) VALUES (?, ?)");
    query.addBindValue(playlistId);
    query.addBindValue(songId);
    query.exec();
}

void PlaylistRepository::removeSong(int playlistId, int songId) {
    QSqlQuery query;
    query.prepare("DELETE FROM playlist_songs WHERE playlistId = ? AND songId = ?");
    query.addBindValue(playlistId);
    query.addBindValue(songId);
    query.exec();
}

QList<Playlist*> PlaylistRepository::playlists(int listenerId) {
    QList<Playlist*> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM playlists WHERE listenerId = ?");
    query.addBindValue(listenerId);
    if (query.exec()) {
        while (query.next()) {
            result.append(new Playlist(query.value(0).toInt(), query.value(1).toString(),
                                       query.value(2).toInt()));
        }
    }
    return result;
}

Account* ArtistRepository::save(Account* entity) {
    QSqlQuery query;
    query.prepare("INSERT INTO accounts (fullName, userName, biography, role, password) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(entity->getFullName());
    query.addBindValue(entity->getUserName());
    query.addBindValue(entity->getBiography());
    query.addBindValue("Artist");
    query.addBindValue(entity->getPassword());

    if (!query.exec()) {
        qDebug() << "Error saving artist:" << query.lastError().text();
    }
    delete entity;
    return nullptr;
}

bool ArtistRepository::remove(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM accounts WHERE id = ? AND role = 'Artist'");
    query.addBindValue(id);
    return query.exec();
}

Account* ArtistRepository::search(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM accounts WHERE id = ? AND role = 'Artist'");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return new Artist(query.value(0).toInt(), query.value(1).toString(),
                          query.value(2).toString(), query.value(3).toString(),
                          query.value(5).toString());
    }
    return nullptr;
}

QList<Account*> ArtistRepository::getAll() {
    QList<Account*> result;
    QSqlQuery query("SELECT * FROM accounts WHERE role = 'Artist'");
    while (query.next()) {
        result.append(new Artist(query.value(0).toInt(), query.value(1).toString(),
                                 query.value(2).toString(), query.value(3).toString(),
                                 query.value(5).toString()));
    }
    return result;
}

Account* ArtistRepository::searchByUserName(QString userName) {
    QSqlQuery query;
    query.prepare("SELECT * FROM accounts WHERE userName = ? AND role = 'Artist'");
    query.addBindValue(userName);
    if (query.exec() && query.next()) {
        return new Artist(query.value(0).toInt(), query.value(1).toString(),
                          query.value(2).toString(), query.value(3).toString(),
                          query.value(5).toString());
    }
    return nullptr;
}

bool ArtistRepository::update(Account* entity) {
    QSqlQuery query;
    query.prepare("UPDATE accounts SET fullName=?, userName=?, biography=?, password=? WHERE id=? AND role='Artist'");
    query.addBindValue(entity->getFullName());
    query.addBindValue(entity->getUserName());
    query.addBindValue(entity->getBiography());
    query.addBindValue(entity->getPassword());
    query.addBindValue(entity->getId());

    if (!query.exec()) {
        qDebug() << "Artist Update Error:" << query.lastError().text();
        return false;
    }
    return true;
}

Account* ListenerRepository::save(Account* entity) {
    QSqlQuery query;
    query.prepare("INSERT INTO accounts (fullName, userName, biography, role, password) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(entity->getFullName());
    query.addBindValue(entity->getUserName());
    query.addBindValue(entity->getBiography());
    query.addBindValue("Listener");
    query.addBindValue(entity->getPassword());

    if (!query.exec()) {
        qDebug() << "Error saving listener:" << query.lastError().text();
    }
    delete entity;
    return nullptr;
}

bool ListenerRepository::remove(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM accounts WHERE id = ? AND role = 'Listener'");
    query.addBindValue(id);
    return query.exec();
}

Account* ListenerRepository::search(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM accounts WHERE id = ? AND role = 'Listener'");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return new Listener(query.value(0).toInt(), query.value(1).toString(),
                            query.value(2).toString(), query.value(3).toString(),
                            query.value(5).toString());
    }
    return nullptr;
}

QList<Account*> ListenerRepository::getAll() {
    QList<Account*> result;
    QSqlQuery query("SELECT * FROM accounts WHERE role = 'Listener'");
    while (query.next()) {
        result.append(new Listener(query.value(0).toInt(), query.value(1).toString(),
                                   query.value(2).toString(), query.value(3).toString(),
                                   query.value(5).toString()));
    }
    return result;
}

Account* ListenerRepository::searchByUserName(QString userName) {
    QSqlQuery query;
    query.prepare("SELECT * FROM accounts WHERE userName = ? AND role = 'Listener'");
    query.addBindValue(userName);
    if (query.exec() && query.next()) {
        return new Listener(query.value(0).toInt(), query.value(1).toString(),
                            query.value(2).toString(), query.value(3).toString(),
                            query.value(5).toString());
    }
    return nullptr;
}

bool ListenerRepository::update(Account* entity) {
    QSqlQuery query;
    query.prepare("UPDATE accounts SET fullName=?, userName=?, biography=?, password=? WHERE id=? AND role='Listener'");
    query.addBindValue(entity->getFullName());
    query.addBindValue(entity->getUserName());
    query.addBindValue(entity->getBiography());
    query.addBindValue(entity->getPassword());
    query.addBindValue(entity->getId());

    if (!query.exec()) {
        qDebug() << "Listener Update Error:" << query.lastError().text();
        return false;
    }
    return true;
}

void ListenerRepository::updateLiked(int listenerId, int songId, bool isLiked) {
    QSqlQuery query;
    if (isLiked) {
        query.prepare("INSERT INTO likes (listenerId, songId) VALUES (?, ?)");
    }
    else {
        query.prepare("DELETE FROM likes WHERE listenerId = ? AND songId = ?");
    }
    query.addBindValue(listenerId);
    query.addBindValue(songId);
    query.exec();
}

bool ListenerRepository::isLiked(int listenerId, int songId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM likes WHERE listenerId = ? AND songId = ?");
    query.addBindValue(listenerId);
    query.addBindValue(songId);
    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}


Album* AlbumRepository::save(Album* entity) {
    QSqlQuery query;
    query.prepare("INSERT INTO albums (name, artistId, coverPath) VALUES (?, ?, ?)");
    query.addBindValue(entity->getName());
    query.addBindValue(entity->getArtistId());
    query.addBindValue(entity->getCoverPath());

    if (query.exec()) {
        delete entity;
        return nullptr;
    }
    return entity;
}

bool AlbumRepository::remove(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM albums WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

Album* AlbumRepository::search(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM albums WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return new Album(query.value(0).toInt(), query.value(1).toString(),
                         query.value(2).toInt(), query.value(3).toString());
    }
    return nullptr;
}

QList<Album*> AlbumRepository::getAll() {
    QList<Album*> result;
    QSqlQuery query("SELECT * FROM albums");
    while (query.next()) {
        result.append(new Album(query.value(0).toInt(), query.value(1).toString(),
                                query.value(2).toInt(), query.value(3).toString()));
    }
    return result;
}

QList<Album*> AlbumRepository::albums(int artistId) {
    QList<Album*> result;
    QSqlQuery query;
    query.prepare("SELECT * FROM albums WHERE artistId = ?");
    query.addBindValue(artistId);
    if (query.exec()) {
        while (query.next()) {
            result.append(new Album(query.value(0).toInt(), query.value(1).toString(),
                                    query.value(2).toInt(), query.value(3).toString()));
        }
    }
    return result;
}