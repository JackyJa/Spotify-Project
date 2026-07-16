#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include "entities.h"
#include "repositories.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ArtistWindow;
}
QT_END_NAMESPACE

class ArtistWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::ArtistWindow* ui;
    Account* currentArtist;
    ArtistRepository* artistRepo;
    SongRepository* songRepo;
    AlbumRepository* albumRepo;

    void refreshAlbums();

private slots:
    void addAlbum();
    void addSong();
    void editAccount();
    void deleteAccount();
    void viewAlbumSongs(QListWidgetItem* item);
    void signOut();

public:
    ArtistWindow(Account* user, ArtistRepository* aRepo, SongRepository* sRepo, QWidget* parent = nullptr);
    ~ArtistWindow();
};