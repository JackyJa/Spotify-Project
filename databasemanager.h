#pragma once

#include <QSqlDatabase>

class DatabaseManager {
private:
    static QSqlDatabase db;
public:
    static bool init();
    static QSqlDatabase getDb();
};