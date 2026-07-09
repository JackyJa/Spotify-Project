#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include "entities.h"
#include "repositories.h"

class ArtistWindow : public QMainWindow {
    Q_OBJECT

private:
    Account* currentArtist;
    ArtistRepository* artistRepo;
    SongRepository* songRepo;
    AlbumRepository* albumRepo;

    QWidget* centralWidget;
    QListWidget* albumsList;
    QPushButton* btnAddAlbum;
    QPushButton* btnAddSong;
    QPushButton* btnEditAccount;
    QPushButton* btnDeleteAccount;

    void setupUI();
    void refreshAlbums();

private slots:
    void addAlbum();
    void addSong();
    void editAccount();
    void deleteAccount();

public:
    ArtistWindow(Account* user, ArtistRepository* aRepo, SongRepository* sRepo, QWidget* parent = nullptr);
    ~ArtistWindow();
};