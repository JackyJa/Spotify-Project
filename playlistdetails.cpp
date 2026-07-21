#include "playlistdetails.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QListWidgetItem>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>

QPixmap makeRoundedPixmap(const QString& path, int size) {
    QPixmap src(path);
    QPixmap roundedPix(size, size);
    roundedPix.fill(Qt::transparent);

    QPainter painter(&roundedPix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath roundPath;
    roundPath.addRoundedRect(0, 0, size, size, 15, 15);
    painter.setClipPath(roundPath);
    painter.drawPixmap(0, 0, src.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    return roundedPix;
}

PlaylistDetailsWindow::PlaylistDetailsWindow(int plId, SongRepository* songRepo, PlaylistRepository* plRepo, ListenerRepository* lRepo, int listenerId, QWidget* parent) : QDialog(parent) {
    this->listenerId = listenerId;
    listenerRepo = lRepo;
    playlistId = plId;
    playlistRepo = plRepo;
    setWindowTitle("Playlist Songs");
    resize(400, 450);

    QVBoxLayout* layout = new QVBoxLayout(this);
    songsList = new QListWidget(this);

    songsList->setViewMode(QListWidget::IconMode);
    songsList->setIconSize(QSize(100, 100));
    songsList->setResizeMode(QListWidget::Adjust);
    songsList->setMovement(QListWidget::Static);
    songsList->setSpacing(15);


    QPushButton* btnRemoveSong = new QPushButton("Remove Selected Song", this);
    btnRemoveSong->setStyleSheet("background-color: #ff4c4c; color: white; border-radius: 15px; padding: 10px; font-weight: bold;");

    layout->addWidget(songsList);
    layout->addWidget(btnRemoveSong);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    if (listenerId != -1) {
        setWindowTitle("Favorite Songs");
        songs = songRepo->getByLikedSongs(listenerId);
        btnRemoveSong->setText("Unlike Selected Song");
    }
    else {
        songs = songRepo->getByPlaylist(playlistId);
    }

    for (int i = 0; i < songs.size(); i++) {
        QListWidgetItem* item = new QListWidgetItem(songs[i]->getName());
        item->setTextAlignment(Qt::AlignCenter);
        QString coverPath = songs[i]->getCoverPath();

        if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
            item->setIcon(QIcon(makeRoundedPixmap(coverPath, 100)));
        }
        songsList->addItem(item);
    }

    connect(songsList, &QListWidget::itemClicked, this, &PlaylistDetailsWindow::onSongClicked);
    connect(btnRemoveSong, &QPushButton::clicked, this, &PlaylistDetailsWindow::onRemoveSongClicked);
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

void PlaylistDetailsWindow::onRemoveSongClicked() {
    int row = songsList->currentRow();
    if (row < 0 || row >= songs.size()) {
        QMessageBox::warning(this, "Error", "Please select a song first.");
        return;
    }

    int songId = songs[row]->getId();


    if (listenerId != -1) {
        listenerRepo->updateLiked(listenerId, songId, false);
    }
    else {
        playlistRepo->removeSong(playlistId, songId);
    }


    delete songs.takeAt(row);;
    delete songsList->takeItem(row);

    QMessageBox::information(this, "Success", "Song removed successfully.");
}