#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include "entities.h"
#include "repositories.h"

class ListenerWindow : public QMainWindow {
    Q_OBJECT

private:
    Account* currentListener;
    ListenerRepository* listenerRepo;
    SongRepository* songRepo;

    QWidget* centralWidget;
    QListWidget* playlistsList;
    QPushButton* btnCreatePlaylist;
    QPushButton* btnViewArtists;
    QPushButton* btnEditAccount;
    QPushButton* btnDeleteAccount;

    void setupUI();
    void refreshPlaylists();

private slots:
    void createPlaylist();
    void viewArtists();
    void editAccount();
    void deleteAccount();

public:
    ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent = nullptr);
    ~ListenerWindow();
};