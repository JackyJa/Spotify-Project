#include "listenerwindow.h"
#include "ui_listenerwindow.h"
#include "chatbotwindow.h"
#include "playlistdetails.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFile>
#include <QPainter>
#include <QPainterPath>

ListenerWindow::ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent) : QMainWindow(parent) {
    ui = new Ui::ListenerWindow;
    currentListener = user;
    listenerRepo = lRepo;
    songRepo = sRepo;
    ui->setupUi(this);
    playlistRepo = new PlaylistRepository();

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    ui->welcomeLabel->setText("Welcome, " + currentListener->getFullName() + "!");

    QString photoPath = currentListener->getProfilePhotoPath();
    if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
        QPixmap pix(photoPath);
        int size = 50;


        QPixmap roundedPix(size, size);
        roundedPix.fill(Qt::transparent);

        QPainter painter(&roundedPix);
        painter.setRenderHint(QPainter::Antialiasing, true);


        QPainterPath path;
        path.addRoundedRect(0, 0, size, size, 25, 25);


        painter.setClipPath(path);
        painter.drawPixmap(0, 0, pix.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));


        ui->profilePhotoLabel->setPixmap(roundedPix);
    }


    connect(ui->btnCreatePlaylist, &QPushButton::clicked, this, &ListenerWindow::createPlaylist);
    connect(ui->btnViewArtists, &QPushButton::clicked, this, &ListenerWindow::viewArtists);
    connect(ui->btnEditAccount, &QPushButton::clicked, this, &ListenerWindow::editAccount);
    connect(ui->btnDeleteAccount, &QPushButton::clicked, this, &ListenerWindow::deleteAccount);
    connect(ui->btnSearchSongs, &QPushButton::clicked, this, &ListenerWindow::searchSongs);
    connect(ui->btnLikeSong, &QPushButton::clicked, this, &ListenerWindow::likeSong);
    connect(ui->btnPlayFromPlaylist, &QPushButton::clicked, this, &ListenerWindow::playFromPlaylist);
    connect(ui->btnOpenChatbot, &QPushButton::clicked, this, &ListenerWindow::openChatbot);
    connect(ui->playlistsList, &QListWidget::itemClicked, this, &ListenerWindow::viewPlaylistSongs);
    connect(ui->btnSignOut, &QPushButton::clicked, this, &ListenerWindow::signOut);
    connect(ui->btnDeletePlaylist, &QPushButton::clicked, this, &ListenerWindow::deletePlaylist);

    refreshPlaylists();
}

ListenerWindow::~ListenerWindow() {
    delete ui;
    delete currentListener;
    delete playlistRepo;
    delete player;
    delete audioOutput;
}

void ListenerWindow::refreshPlaylists() {
    ui->playlistsList->clear();
    ui->playlistsList->addItem("Favorite Songs");

    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    for (int i = 0; i < myPlaylists.size(); i++) {
        ui->playlistsList->addItem(myPlaylists[i]->getName());
        delete myPlaylists[i];
    }
}

void ListenerWindow::createPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(this, "Create Playlist", "Playlist Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        playlistRepo->save(new Playlist(0, name, currentListener->getId()));
        QMessageBox::information(this, "Success", "Playlist created successfully.");
        refreshPlaylists();
    }
}

void ListenerWindow::viewArtists() {
    ArtistRepository tempArtistRepo;
    QList<Account*> artists = tempArtistRepo.getAll();
    if (artists.isEmpty()) {
        QMessageBox::information(this, "Info", "No artists found.");
        return;
    }
    QStringList artistNames;
    for (int i = 0; i < artists.size(); i++) {
        artistNames << artists[i]->getFullName();
    }

    bool ok;
    QString artistChoice = QInputDialog::getItem(this, "View Artists", "Select an Artist:", artistNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < artists.size(); i++) {
            delete artists[i];
        }
        return;
    }

    int artistId = -1;
    for (int i = 0; i < artists.size(); i++) {
        if (artists[i]->getFullName() == artistChoice) artistId = artists[i]->getId();
        delete artists[i];
    }
    if (artistId == -1) return;

    QList<Song*> songs = songRepo->getByArtist(artistId);
    if (songs.isEmpty()) {
        QMessageBox::information(this, "Info", "This artist has no songs.");
        return;
    }
    QStringList songNames;
    for (int i = 0; i < songs.size(); i++) {
        songNames << songs[i]->getName();
    }

    QString songChoice = QInputDialog::getItem(this, "Select Song", "Select a song to add to playlist:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < songs.size(); i++) {
            delete songs[i];
        }
        return;
    }

    int songId = -1;
    for (int i = 0; i < songs.size(); i++) {
        if (songs[i]->getName() == songChoice) {
            songId = songs[i]->getId();
        }
        delete songs[i];
    }
    if (songId == -1) return;

    QList<Playlist*> playlists = playlistRepo->playlists(currentListener->getId());
    if (playlists.isEmpty()) {
        QMessageBox::warning(this, "Error", "Create a playlist first.");
        return;
    }
    QStringList playlistNames;
    for (int i = 0; i < playlists.size(); i++) {
        playlistNames << playlists[i]->getName();
    }

    QString playlistChoice = QInputDialog::getItem(this, "Select Playlist", "Add to which playlist?", playlistNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < playlists.size(); i++) {
            delete playlists[i];
        }
        return;
    }

    int playlistId = -1;
    for (int i = 0; i < playlists.size(); i++) {
        if (playlists[i]->getName() == playlistChoice) playlistId = playlists[i]->getId();
        delete playlists[i];
    }
    if (playlistId != -1) {
        playlistRepo->insertSong(playlistId, songId);
        QMessageBox::information(this, "Success", "Song added to playlist!");
    }
}

void ListenerWindow::searchSongs() {
    bool ok;
    QString name = QInputDialog::getText(this, "Search", "Song Name:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString genre = QInputDialog::getText(this, "Search", "Genre:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    QString yearStr = QInputDialog::getText(this, "Search", "Release Year:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    int year = 0;
    if (!yearStr.isEmpty()) year = yearStr.toInt();

    QList<Song*> songs = songRepo->searchSongs(name, genre, year);
    if (songs.isEmpty()) { QMessageBox::information(this, "Result", "No songs found."); return; }
    QStringList songNames;
    for (int i = 0; i < songs.size(); i++) {
        songNames << songs[i]->getName();
    }
    QInputDialog::getItem(this, "Search Results", "Found songs:", songNames, 0, false, &ok);
    for (int i = 0; i < songs.size(); i++){
        delete songs[i];
    }
}

void ListenerWindow::likeSong() {
    QList<Song*> allSongs = songRepo->getAll();
    if (allSongs.isEmpty()) {
        QMessageBox::information(this, "Info", "No songs.");
        return;
    }
    QStringList songNames;
    for (int i = 0; i < allSongs.size(); i++) {
        songNames << allSongs[i]->getName();
    }
    bool ok;
    QString choice = QInputDialog::getItem(this, "Like Song", "Select:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < allSongs.size(); i++) {
            delete allSongs[i];
        }
        return;
    }
    for (int i = 0; i < allSongs.size(); i++) {
        if (allSongs[i]->getName() == choice) listenerRepo->updateLiked(currentListener->getId(), allSongs[i]->getId(), true);
        delete allSongs[i];
    }
    QMessageBox::information(this, "Success", "Added to Favorites!");
}

void ListenerWindow::playFromPlaylist() {
    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    if (myPlaylists.isEmpty()) {
        QMessageBox::warning(this, "Error", "No playlists.");
        return;
    }
    QStringList plNames;
    for (int i = 0; i < myPlaylists.size(); ++i){
        plNames << myPlaylists[i]->getName();
    }
    bool ok;
    QString plChoice = QInputDialog::getItem(this, "Play", "Playlist:", plNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < myPlaylists.size(); i++){
            delete myPlaylists[i];
        }
        return;
    }
    int plId = -1;
    for (int i = 0; i < myPlaylists.size(); i++) {
        if (myPlaylists[i]->getName() == plChoice) plId = myPlaylists[i]->getId();
        delete myPlaylists[i];
    }
    if (plId == -1) return;

    QList<Song*> songs = songRepo->getByPlaylist(plId);
    if (songs.isEmpty()) {
        QMessageBox::information(this, "Info", "Empty playlist.");
        return;
    }
    QStringList songNames;
    for (int i = 0; i < songs.size(); i++) {
        songNames << songs[i]->getName();
    }
    QString songChoice = QInputDialog::getItem(this, "Play", "Song:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < songs.size(); i++){
            delete songs[i];
        }
        return;
    }
    for (int i = 0; i < songs.size(); i++) {
        if (songs[i]->getName() == songChoice) {
            QString path = songs[i]->getAudioFilePath();
            if (QFile::exists(path)) {
                player->setSource(QUrl::fromLocalFile(path));
                player->play();
                QMessageBox::information(this, "Now Playing", "Playing: " + songs.at(i)->getName());
            }
            else QMessageBox::warning(this, "Error", "File not found.");
        }
        delete songs[i];
    }
}

void ListenerWindow::viewPlaylistSongs(QListWidgetItem* item) {
    if (item->text() == "Favorite Songs") {
        PlaylistDetailsWindow* details = new PlaylistDetailsWindow(-1, songRepo, playlistRepo, listenerRepo, currentListener->getId());
        details->setAttribute(Qt::WA_DeleteOnClose);
        details->exec();
        return;
    }
    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    int playlistId = -1;
    for (int i = 0; i < myPlaylists.size(); i++) {
        if (myPlaylists[i]->getName() == item->text()) playlistId = myPlaylists[i]->getId();
        delete myPlaylists[i];
    }
    if (playlistId == -1) {
        return;
    }
    PlaylistDetailsWindow* details = new PlaylistDetailsWindow(playlistId, songRepo, playlistRepo);
    details->setAttribute(Qt::WA_DeleteOnClose);
    details->exec();
}

void ListenerWindow::editAccount() {
    bool ok;
    QString fullName = QInputDialog::getText(this, "Edit", "Full Name:", QLineEdit::Normal, currentListener->getFullName(), &ok);
    if (!ok || fullName.isEmpty()) return;
    QString userName = QInputDialog::getText(this, "Edit", "Username:", QLineEdit::Normal, currentListener->getUserName(), &ok);
    if (!ok || userName.isEmpty()) return;
    QString bio = QInputDialog::getText(this, "Edit", "Biography:", QLineEdit::Normal, currentListener->getBiography(), &ok);
    if (!ok || bio.isEmpty()) return;
    QString password = QInputDialog::getText(this, "Edit", "New Password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QRegularExpression strongRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*]).{12,}");
    QRegularExpression mediumRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9!@#$%^&*]).{6,}");
    QString strength;
    if (strongRegex.match(password).hasMatch()) strength = "Strong";
    else if (mediumRegex.match(password).hasMatch()) strength = "Medium";
    else strength = "Weak";
    if (strength == "Weak") { QMessageBox::warning(this, "Error", "Password too weak."); return; }

    currentListener->setFullName(fullName);
    currentListener->setUserName(userName);
    currentListener->setBiography(bio);
    currentListener->setPassword(password);
    if (listenerRepo->update(currentListener)) QMessageBox::information(this, "Success", "Account updated.");
    else QMessageBox::warning(this, "Error", "Update failed.");
}

void ListenerWindow::deleteAccount() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete", "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        listenerRepo->remove(currentListener->getId());
        this->signOut();
    }
}

void ListenerWindow::openChatbot() {
    ChatbotWindow* chat = new ChatbotWindow();
    chat->setAttribute(Qt::WA_DeleteOnClose);
    chat->show();
}

void ListenerWindow::signOut() {
    MainWindow* login = new MainWindow();
    login->show();
    this->close();
}

void ListenerWindow::deletePlaylist() {
    QListWidgetItem* selectedItem = ui->playlistsList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Error", "Please select a playlist from the list first.");
        return;
    }
    if (selectedItem->text() == "Favorite Songs") {
        QMessageBox::warning(this, "Error", "Favorite Songs list cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Playlist", "Are you sure you want to delete this playlist?", QMessageBox::Yes|QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    int playlistId = -1;
    for (int i = 0; i < myPlaylists.size(); i++) {
        if (myPlaylists[i]->getName() == selectedItem->text()) {
            playlistId = myPlaylists[i]->getId();
        }
        delete myPlaylists[i];
    }

    if (playlistId != -1) {
        playlistRepo->remove(playlistId);
        QMessageBox::information(this, "Success", "Playlist deleted successfully.");
        refreshPlaylists();
    }
}