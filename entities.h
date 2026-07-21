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
    Account(int accId, QString fn, QString un, QString bio, QString pass, QString photo = "") {
        id = accId;
        fullName = fn;
        userName = un;
        biography = bio;
        password = pass;
        profilePhotoPath = photo;
    }

    virtual ~Account() {}

    int getId() const { return id; }
    QString getFullName() const { return fullName; }
    QString getUserName() const { return userName; }
    QString getBiography() const { return biography; }
    QString getPassword() const { return password; }
    QString getProfilePhotoPath() const { return profilePhotoPath; }
    virtual QString getRole() const = 0;

    void setFullName(QString fn) { fullName = fn; }
    void setUserName(QString un) { userName = un; }
    void setPassword(QString pass) { password = pass; }
    void setBiography(QString bio) { biography = bio; }
    void setProfilePhotoPath(QString photo) { profilePhotoPath = photo; }


};

class Artist : public Account {
public:
    Artist(int accId, QString fn, QString un, QString bio, QString pass, QString photo = "") : Account(accId, fn, un, bio, pass, photo) {}

    QString getRole() const override {
        return "Artist";
    }
};

class Listener : public Account {
public:
    Listener(int accId, QString fn, QString un, QString bio, QString pass, QString photo = "") : Account(accId, fn, un, bio, pass, photo) {}

    QString getRole() const override {
        return "Listener";
    }
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
    QString coverPath;


public:
    Song(int sId, QString n, int year, QString g, QString path, int aId, int alId, QString cover = "") {
        id = sId;
        name = n;
        releaseYear = year;
        genre = g;
        audioFilePath = path;
        artistId = aId;
        albumId = alId;
        coverPath = cover;
    }

    int getId() const { return id; }
    QString getName() const { return name; }
    int getArtistId() const { return artistId; }
    int getAlbumId() const { return albumId; }
    QString getAudioFilePath() const { return audioFilePath; }
    int getReleaseYear() const { return releaseYear; }
    QString getGenre() const { return genre; }
    QString getCoverPath() const { return coverPath; }

    void setName(QString n) { name = n; }
    void setReleaseYear(int y) { releaseYear = y; }
    void setGenre(QString g) { genre = g; }
    void setCoverPath(QString c) { coverPath = c; }

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
    QString getCoverPath() const { return coverPath; }
    void setName(QString n) { name = n; }
    void setCoverPath(QString c) { coverPath = c; }
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