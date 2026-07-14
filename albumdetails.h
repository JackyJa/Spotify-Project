#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include "repositories.h"

class AlbumDetailsWindow : public QDialog {
    Q_OBJECT

private:
    QTableWidget* tableWidget;

public:
    AlbumDetailsWindow(int albumId, int artistId, SongRepository* songRepo, QWidget* parent = nullptr);
};