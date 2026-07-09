#include "repositories.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<Song*> SongRepository::getAll() { return QList<Song*>(); }
Song* SongRepository::save(Song* entity) { return entity; }
bool SongRepository::remove(int id) { return false; }
Song* SongRepository::search(int id) { return nullptr; }
QList<Song*> SongRepository::singleSongs(int artistId) { return QList<Song*>(); }
QList<Song*> SongRepository::getByAlbum(int albumId) { return QList<Song*>(); }
QList<Song*> SongRepository::getByArtist(int artistId) { return QList<Song*>(); }

QList<Playlist*> PlaylistRepository::getAll() { return QList<Playlist*>(); }
Playlist* PlaylistRepository::save(Playlist* entity) { return entity; }
bool PlaylistRepository::remove(int id) { return false; }
Playlist* PlaylistRepository::search(int id) { return nullptr; }
void PlaylistRepository::insertSong(int playlistId, int songId) {}
void PlaylistRepository::removeSong(int playlistId, int songId) {}
QList<Playlist*> PlaylistRepository::playlists(int listenerId) { return QList<Playlist*>(); }

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

void ListenerRepository::updateLiked(int listenerId, int songId, bool isLiked) {}
bool ListenerRepository::isLiked(int listenerId, int songId) { return false; }