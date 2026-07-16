#include "playlistdetails.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>

PlaylistDetailsWindow::PlaylistDetailsWindow(int playlistId, SongRepository* songRepo, QWidget* parent) : QDialog(parent) {

    setWindowTitle("Playlist Songs");
    resize(400, 400);

    this->setStyleSheet("QDialog { background-color: #121212; }"
                        "QListWidget { background-color: #181818; color: #FFFFFF; border: none; border-radius: 8px; font-size: 16px; padding: 10px; }"
                        "QListWidget::item { padding: 15px; border-bottom: 1px solid #282828; }"
                        "QListWidget::item:selected { background-color: #1DB954; color: #000000; font-weight: bold; }");



    QVBoxLayout* layout = new QVBoxLayout(this);
    songsList = new QListWidget(this);
    layout->addWidget(songsList);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    songs = songRepo->getByPlaylist(playlistId);
    for (int i = 0; i < songs.size(); i++) {
        songsList->addItem(songs[i]->getName());
    }

    connect(songsList, &QListWidget::itemClicked, this, &PlaylistDetailsWindow::onSongClicked);
}

PlaylistDetailsWindow::~PlaylistDetailsWindow() {
    for (int i = 0; i < songs.size(); i++) {
        delete songs[i];
    }
}

void PlaylistDetailsWindow::onSongClicked(QListWidgetItem* item) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Play Music", "Do you want to play this song?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int row = songsList->row(item);
        if (row >= 0 && row < songs.size()) {
            QString path = songs[row]->getAudioFilePath();
            if (QFile::exists(path)) {
                player->setSource(QUrl::fromLocalFile(path));
                player->play();
                QMessageBox::information(this, "Now Playing", "Playing: " + songs.at(row)->getName());
            }
            else {
                QMessageBox::warning(this, "Error", "Audio file not found on disk.");
            }
        }
    }
}