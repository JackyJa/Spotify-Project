#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "entities.h"
#include "repositories.h"

class ListenerWindow : public QMainWindow {
    Q_OBJECT

private:
    Account* currentListener;
    ListenerRepository* listenerRepo;
    SongRepository* songRepo;
    PlaylistRepository* playlistRepo;

    QWidget* centralWidget;
    QListWidget* playlistsList;
    QPushButton* btnCreatePlaylist;
    QPushButton* btnViewArtists;
    QPushButton* btnEditAccount;
    QPushButton* btnDeleteAccount;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QPushButton* btnPlayFromPlaylist;

    void setupUI();
    void refreshPlaylists();

private slots:
    void playFromPlaylist();
    void createPlaylist();
    void viewArtists();
    void editAccount();
    void deleteAccount();

public:
    ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent = nullptr);
    ~ListenerWindow();
};