#pragma once

#include <QString>
#include <QList>

class Account {
private:
    int id;
    QString fullName;
    QString userName;
    QString biography;
    QString password;
    QString profilePhotoPath;

public:
    Account(int accId, QString fn, QString un, QString bio, QString pass){
        id = accId;
        fullName = fn;
        userName = un;
        biography = bio;
        password = pass;
    }

    virtual ~Account() {}

    int getId() const { return id; }
    QString getFullName() const { return fullName; }
    QString getUserName() const { return userName; }
    QString getBiography() const { return biography; }
    QString getPassword() const { return password; }

    void setFullName(QString fn) { fullName = fn; }
    void setUserName(QString un) { userName = un; }
    void setPassword(QString pass) { password = pass; }

    virtual QString getRole() const = 0;
};

class Artist : public Account {
public:
    Artist(int accId, QString fn, QString un, QString bio, QString pass)
        : Account(accId, fn, un, bio, pass) {}

    QString getRole() const override { return "Artist"; }
};

class Listener : public Account {
public:
    Listener(int accId, QString fn, QString un, QString bio, QString pass)
        : Account(accId, fn, un, bio, pass) {}

    QString getRole() const override { return "Listener"; }
};

class Song {
private:
    int id;
    QString name;
    int releaseYear;
    QString genre;
    QString audioFilePath;
    int artistId;
    int albumId;

public:
    Song(int sId, QString n, int year, QString g, QString path, int aId, int alId) {
        id = sId;
        name = n;
        releaseYear = year;
        genre = g;
        audioFilePath = path;
        artistId = aId;
        albumId = alId;
    }

    int getId() const { return id; }
    QString getName() const { return name; }
    int getArtistId() const { return artistId; }
    int getAlbumId() const { return albumId; }
    void setName(QString n) { name = n; }
};

class Album {
private:
    int id;
    QString name;
    int artistId;
    QString coverPath;

public:
    Album(int aId, QString n, int artId, QString cover) {
        id = aId;
        name = n;
        artistId = artId;
        coverPath = cover;
    }

    int getId() const { return id; }
    QString getName() const { return name; }
    int getArtistId() const { return artistId; }
};

class Playlist {
private:
    int id;
    QString name;
    int listenerId;
    QList<int> songIds;

public:
    Playlist(int pId, QString n, int lId) {
        id = pId;
        name = n;
        listenerId = lId;
    }

    int getId() const { return id; }
    QString getName() const { return name; }
    int getListenerId() const { return listenerId; }
    QList<int> getSongs() const { return songIds; }
    void addSong(int sId) { songIds.append(sId); }
    void removeSong(int sId) { songIds.removeAll(sId); }
};