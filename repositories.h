#pragma once

#include "entities.h"
#include <QList>
#include <stdexcept>

template <typename T>
class AbstractRepository {
public:
    virtual T* save(T* entity) = 0;
    virtual bool remove(int id) = 0;
    virtual T* search(int id) = 0;
    virtual QList<T*> getAll() = 0;
    virtual ~AbstractRepository() {}
};

class SongRepository : public AbstractRepository<Song> {
private:
    QList<Song*> storage;
    int nextId;
public:
    SongRepository() : nextId(1) {}
    Song* save(Song* entity) override;
    bool remove(int id) override;
    Song* search(int id) override;
    QList<Song*> getAll() override { return storage; }
    QList<Song*> singleSongs(int artistId);
    QList<Song*> getByAlbum(int albumId);
    QList<Song*> getByArtist(int artistId);
};

class PlaylistRepository : public AbstractRepository<Playlist> {
private:
    QList<Playlist*> storage;
    int nextId;
public:
    PlaylistRepository() : nextId(1) {}
    Playlist* save(Playlist* entity) override;
    bool remove(int id) override;
    Playlist* search(int id) override;
    QList<Playlist*> getAll() override { return storage; }
    void insertSong(int playlistId, int songId);
    void removeSong(int playlistId, int songId);
    QList<Playlist*> playlists(int listenerId);
};

class AccountRepository : public AbstractRepository<Account> {
public:
    virtual Account* searchByUserName(QString userName) = 0;
};

class ArtistRepository : public AccountRepository {
private:
    QList<Artist*> storage;
    int nextId;
public:
    ArtistRepository() : nextId(1) {}
    Account* save(Account* entity) override;
    bool remove(int id) override;
    Account* search(int id) override;
    QList<Account*> getAll() override;
    Account* searchByUserName(QString userName) override;
};

class ListenerRepository : public AccountRepository {
private:
    QList<Listener*> storage;
    int nextId;
public:
    ListenerRepository() : nextId(1) {}
    Account* save(Account* entity) override;
    bool remove(int id) override;
    Account* search(int id) override;
    QList<Account*> getAll() override;
    Account* searchByUserName(QString userName) override;
    void updateLiked(int listenerId, int songId, bool isLiked);
    bool isLiked(int listenerId, int songId);
};