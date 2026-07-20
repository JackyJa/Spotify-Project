#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QListWidgetItem>
#include <QAudioOutput>
#include "entities.h"
#include "repositories.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ListenerWindow;
}
QT_END_NAMESPACE

class ListenerWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::ListenerWindow* ui;
    Account* currentListener;
    ListenerRepository* listenerRepo;
    SongRepository* songRepo;
    PlaylistRepository* playlistRepo;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    void refreshPlaylists();

private slots:
    void createPlaylist();
    void viewArtists();
    void editAccount();
    void deleteAccount();
    void searchSongs();
    void likeSong();
    void playFromPlaylist();
    void openChatbot();
    void viewPlaylistSongs(QListWidgetItem* item);
    void signOut();
    void deletePlaylist();

public:
    ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent = nullptr);
    ~ListenerWindow();
};