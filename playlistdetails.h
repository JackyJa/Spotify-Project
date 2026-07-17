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

private slots:
    void onSongClicked(QListWidgetItem* item);

public:
    PlaylistDetailsWindow(int playlistId, SongRepository* songRepo, int listenerId = -1, QWidget* parent = nullptr);
    ~PlaylistDetailsWindow();
};