#include "playlistdetails.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>

PlaylistDetailsWindow::PlaylistDetailsWindow(int playlistId, SongRepository* songRepo, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Playlist Songs");
    resize(400, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    songsList = new QListWidget(this);
    layout->addWidget(songsList);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    songs = songRepo->getByPlaylist(playlistId);
    for (int i = 0; i < songs.size(); ++i) {
        songsList->addItem(songs.at(i)->getName());
    }

    connect(songsList, &QListWidget::itemClicked, this, &PlaylistDetailsWindow::onSongClicked);
}

PlaylistDetailsWindow::~PlaylistDetailsWindow() {
    for (int i = 0; i < songs.size(); ++i) {
        delete songs.at(i);
    }
}

void PlaylistDetailsWindow::onSongClicked(QListWidgetItem* item) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Play Music", "Do you want to play this song?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int row = songsList->row(item);
        if (row >= 0 && row < songs.size()) {
            QString path = songs.at(row)->getAudioFilePath();
            if (QFile::exists(path)) {
                player->setSource(QUrl::fromLocalFile(path));
                player->play();
                QMessageBox::information(this, "Now Playing", "Playing: " + songs.at(row)->getName());
            } else {
                QMessageBox::warning(this, "Error", "Audio file not found on disk.");
            }
        }
    }
}