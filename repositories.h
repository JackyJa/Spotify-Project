#pragma once

#include "entities.h"
#include <QList>

template <typename T>
class AbstractRepository {
public:
    virtual T* save(T* entity) = 0;
    virtual bool remove(int id) = 0;
    virtual T* search(int id) = 0;
    virtual QList<T*> getAll() = 0;
    virtual ~AbstractRepository() {};
};

class SongRepository : public AbstractRepository<Song> {
public:
    Song* save(Song* entity) override;
    bool remove(int id) override;
    Song* search(int id) override;
    QList<Song*> getAll() override;
    QList<Song*> singleSongs(int artistId);
    QList<Song*> getByAlbum(int albumId);
    QList<Song*> getByArtist(int artistId);
    QList<Song*> getByPlaylist(int playlistId);
    QList<Song*> searchSongs(QString name, QString genre, int year);
    QList<Song*> getByLikedSongs(int listenerId);
    bool removeByAlbum(int albumId);
    bool update(Song* entity);
};

class PlaylistRepository : public AbstractRepository<Playlist> {
public:
    Playlist* save(Playlist* entity) override;
    bool remove(int id) override;
    Playlist* search(int id) override;
    QList<Playlist*> getAll() override;
    void insertSong(int playlistId, int songId);
    void removeSong(int playlistId, int songId);
    QList<Playlist*> playlists(int listenerId);
};

class AccountRepository : public AbstractRepository<Account> {
public:
    virtual Account* searchByUserName(QString userName) = 0;
};

class ArtistRepository : public AccountRepository {
public:
    Account* save(Account* entity) override;
    bool remove(int id) override;
    bool update(Account* entity);
    Account* search(int id) override;
    QList<Account*> getAll() override;
    Account* searchByUserName(QString userName) override;
};

class ListenerRepository : public AccountRepository {
public:
    Account* save(Account* entity) override;
    bool remove(int id) override;
    bool update(Account* entity);
    Account* search(int id) override;
    QList<Account*> getAll() override;
    Account* searchByUserName(QString userName) override;
    void updateLiked(int listenerId, int songId, bool isLiked);
    bool isLiked(int listenerId, int songId);
};

class AlbumRepository : public AbstractRepository<Album> {
public:
    Album* save(Album* entity) override;
    bool remove(int id) override;
    Album* search(int id) override;
    QList<Album*> getAll() override;
    QList<Album*> albums(int artistId);
    bool update(Album* entity);
};