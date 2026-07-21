#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include "repositories.h"

class AlbumDetailsWindow : public QDialog {
    Q_OBJECT

private:
    QTableWidget* tableWidget;
    QList<Song*> songs;
    SongRepository* songRepo;
    int currentAlbumId;
    int currentArtistId;

    void refreshTable();

private slots:
    void onEditSongClicked();
    void onDeleteSongClicked();

public:
    AlbumDetailsWindow(int albumId, int artistId, SongRepository* songRepo, QWidget* parent = nullptr);
    ~AlbumDetailsWindow();
};