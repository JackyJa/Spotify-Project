#pragma once

#include <QDialog>
#include <QListWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "repositories.h"

class PlaylistDetailsWindow : public QDialog {
    Q_OBJECT

private:
    QListWidget* songsList;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QList<Song*> songs;
    int playlistId;
    PlaylistRepository* playlistRepo;
    int listenerId;
    ListenerRepository* listenerRepo;

private slots:
    void onSongClicked(QListWidgetItem* item);
    void onRemoveSongClicked();

public:
    PlaylistDetailsWindow(int plId, SongRepository* songRepo, PlaylistRepository* plRepo, ListenerRepository* lRepo = nullptr, int listenerId = -1, QWidget* parent = nullptr);
    ~PlaylistDetailsWindow();
};