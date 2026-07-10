#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DatabaseManager::db = QSqlDatabase::database();

bool DatabaseManager::init() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("spotify.db");

    if (!db.open()) {
        qDebug() << "Error: Could not open database";
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS accounts ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "fullName TEXT, "
               "userName TEXT UNIQUE, "
               "biography TEXT, "
               "role TEXT, "
               "password TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS albums ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "artistId INTEGER, "
               "coverPath TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS songs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "releaseYear INTEGER, "
               "genre TEXT, "
               "audioFilePath TEXT, "
               "artistId INTEGER, "
               "albumId INTEGER, "
               "coverPath TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS playlists ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT, "
               "listenerId INTEGER)");

    query.exec("CREATE TABLE IF NOT EXISTS playlist_songs ("
               "playlistId INTEGER, "
               "songId INTEGER)");

    query.exec("CREATE TABLE IF NOT EXISTS likes ("
               "listenerId INTEGER, "
               "songId INTEGER)");

    return true;
}

QSqlDatabase DatabaseManager::getDb() {
    return db;
}