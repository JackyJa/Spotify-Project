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

    return true;
}

QSqlDatabase DatabaseManager::getDb() {
    return db;
}