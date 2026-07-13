#include "listenerwindow.h"
#include "repositories.h"
#include <qlabel.h>
#include <QRegularExpression>
#include <QFile>
#include <QDebug>
#include <QInputDialog>

ListenerWindow::ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent)
    : QMainWindow(parent), currentListener(user), listenerRepo(lRepo), songRepo(sRepo) {

    playlistRepo = new PlaylistRepository();
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);
    connect(player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
        if (error != QMediaPlayer::NoError) {
            QMessageBox::critical(nullptr, "Media Player Error", "Could not play audio.\nError: " + errorString);
            qDebug() << "Media Error:" << errorString;
        }
    });
    setWindowTitle("Listener Dashboard - " + currentListener->getUserName());
    resize(500, 400);
    setupUI();
    refreshPlaylists();
}

ListenerWindow::~ListenerWindow() {
    delete currentListener;
    delete playlistRepo;
    delete player;
    delete audioOutput;
}

void ListenerWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel* welcomeLabel = new QLabel("Welcome Listener: " + currentListener->getFullName(), centralWidget);

    playlistsList = new QListWidget(centralWidget);
    btnCreatePlaylist = new QPushButton("Create New Playlist", centralWidget);
    btnViewArtists = new QPushButton("View Artists & Albums", centralWidget);
    btnEditAccount = new QPushButton("Edit Account", centralWidget);
    btnDeleteAccount = new QPushButton("Delete Account", centralWidget);
    btnOpenChatbot = new QPushButton("Ask AI Assistant", centralWidget);
    layout->addWidget(btnOpenChatbot);
    connect(btnOpenChatbot, &QPushButton::clicked, this, &ListenerWindow::openChatbot);

    layout->addWidget(welcomeLabel);
    layout->addWidget(new QLabel("Your Playlists:"));
    layout->addWidget(playlistsList);
    layout->addWidget(btnCreatePlaylist);
    layout->addWidget(btnViewArtists);
    layout->addWidget(btnEditAccount);
    layout->addWidget(btnDeleteAccount);
    btnPlayFromPlaylist = new QPushButton("Play Song from Playlist", centralWidget);
    layout->addWidget(btnPlayFromPlaylist);
    connect(btnPlayFromPlaylist, &QPushButton::clicked, this, &ListenerWindow::playFromPlaylist);
    btnSearchSongs = new QPushButton("Search & Filter Songs", centralWidget);
    btnLikeSong = new QPushButton("Like a Song", centralWidget);
    layout->addWidget(btnSearchSongs);
    layout->addWidget(btnLikeSong);

    connect(btnSearchSongs, &QPushButton::clicked, this, &ListenerWindow::searchSongs);
    connect(btnLikeSong, &QPushButton::clicked, this, &ListenerWindow::likeSong);

    setCentralWidget(centralWidget);

    connect(btnCreatePlaylist, &QPushButton::clicked, this, &ListenerWindow::createPlaylist);
    connect(btnViewArtists, &QPushButton::clicked, this, &ListenerWindow::viewArtists);
    connect(btnEditAccount, &QPushButton::clicked, this, &ListenerWindow::editAccount);
    connect(btnDeleteAccount, &QPushButton::clicked, this, &ListenerWindow::deleteAccount);
}

void ListenerWindow::refreshPlaylists() {
    playlistsList->clear();
    playlistsList->addItem("Favorite Songs");

    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    for (int i = 0; i < myPlaylists.size(); ++i) {
        playlistsList->addItem(myPlaylists.at(i)->getName());
    }
}


void ListenerWindow::createPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(this, "Create Playlist", "Playlist Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        playlistRepo->save(new Playlist(0, name, currentListener->getId()));
        QMessageBox::information(this, "Success", "Playlist created successfully in database.");
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
    for (int i = 0; i < artists.size(); ++i) {
        artistNames << artists.at(i)->getFullName();
    }

    bool ok;
    QString artistChoice = QInputDialog::getItem(this, "View Artists", "Select an Artist:", artistNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < artists.size(); ++i) delete artists.at(i);
        return;
    }

    int artistId = -1;
    for (int i = 0; i < artists.size(); ++i) {
        if (artists.at(i)->getFullName() == artistChoice) {
            artistId = artists.at(i)->getId();
        }
        delete artists.at(i);
    }

    if (artistId == -1) return;

    QList<Song*> songs = songRepo->getByArtist(artistId);
    if (songs.isEmpty()) {
        QMessageBox::information(this, "Info", "This artist has no songs yet.");
        return;
    }

    QStringList songNames;
    for (int i = 0; i < songs.size(); ++i) {
        songNames << songs.at(i)->getName();
    }

    QString songChoice = QInputDialog::getItem(this, "Select Song", "Select a song to add to your playlist:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < songs.size(); ++i) delete songs.at(i);
        return;
    }

    int songId = -1;
    for (int i = 0; i < songs.size(); ++i) {
        if (songs.at(i)->getName() == songChoice) {
            songId = songs.at(i)->getId();
        }
        delete songs.at(i);
    }

    if (songId == -1) return;

    QList<Playlist*> playlists = playlistRepo->playlists(currentListener->getId());
    if (playlists.isEmpty()) {
        QMessageBox::warning(this, "Error", "You have no playlists. Please create one first.");
        return;
    }

    QStringList playlistNames;
    for (int i = 0; i < playlists.size(); ++i) {
        playlistNames << playlists.at(i)->getName();
    }

    QString playlistChoice = QInputDialog::getItem(this, "Select Playlist", "Add to which playlist?", playlistNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < playlists.size(); ++i) delete playlists.at(i);
        return;
    }

    int playlistId = -1;
    for (int i = 0; i < playlists.size(); ++i) {
        if (playlists.at(i)->getName() == playlistChoice) {
            playlistId = playlists.at(i)->getId();
        }
        delete playlists.at(i);
    }

    if (playlistId != -1) {
        playlistRepo->insertSong(playlistId, songId);
        QMessageBox::information(this, "Success", "Song added to playlist successfully!");
    }
}

void ListenerWindow::editAccount() {
    bool ok;
    QString fullName = QInputDialog::getText(this, "Edit Account", "Full Name:", QLineEdit::Normal, currentListener->getFullName(), &ok);
    if (!ok || fullName.isEmpty()) return;

    QString userName = QInputDialog::getText(this, "Edit Account", "Username:", QLineEdit::Normal, currentListener->getUserName(), &ok);
    if (!ok || userName.isEmpty()) return;

    QString password = QInputDialog::getText(this, "Edit Account", "New Password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QRegularExpression strongRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*]).{12,}");
    QRegularExpression mediumRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9!@#$%^&*]).{6,}");

    QString strength;
    if (strongRegex.match(password).hasMatch()) strength = "Strong";
    else if (mediumRegex.match(password).hasMatch()) strength = "Medium";
    else strength = "Weak";

    QMessageBox::information(this, "Password Strength", "Your password strength is: " + strength);

    if (strength == "Weak") {
        QMessageBox::warning(this, "Error", "Password is too weak. Please use at least 6 chars with upper/lower case and numbers.");
        return;
    }

    currentListener->setFullName(fullName);
    currentListener->setUserName(userName);
    currentListener->setPassword(password);

    listenerRepo->update(currentListener);
    QMessageBox::information(this, "Success", "Account updated in database.");
}

void ListenerWindow::playFromPlaylist() {
    QList<Playlist*> myPlaylists = playlistRepo->playlists(currentListener->getId());
    if (myPlaylists.isEmpty()) {
        QMessageBox::warning(this, "Error", "You have no playlists.");
        return;
    }

    QStringList plNames;
    for (int i = 0; i < myPlaylists.size(); ++i) plNames << myPlaylists.at(i)->getName();

    bool ok;
    QString plChoice = QInputDialog::getItem(this, "Play Music", "Select Playlist:", plNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < myPlaylists.size(); ++i) delete myPlaylists.at(i);
        return;
    }

    int plId = -1;
    for (int i = 0; i < myPlaylists.size(); ++i) {
        if (myPlaylists.at(i)->getName() == plChoice) plId = myPlaylists.at(i)->getId();
        delete myPlaylists.at(i);
    }

    if (plId == -1) return;

    QList<Song*> songs = songRepo->getByPlaylist(plId);
    if (songs.isEmpty()) {
        QMessageBox::information(this, "Info", "This playlist is empty.");
        return;
    }

    QStringList songNames;
    for (int i = 0; i < songs.size(); ++i) songNames << songs.at(i)->getName();

    QString songChoice = QInputDialog::getItem(this, "Play Music", "Select Song:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < songs.size(); ++i) delete songs.at(i);
        return;
    }

    for (int i = 0; i < songs.size(); ++i) {
        if (songs.at(i)->getName() == songChoice) {

            for (int i = 0; i < songs.size(); ++i) {
                if (songs.at(i)->getName() == songChoice) {
                    QString path = songs.at(i)->getAudioFilePath();

                    if (!QFile::exists(path)) {
                        QMessageBox::warning(this, "Error", "Audio file not found on disk:\n" + path);
                        return;
                    }

                    player->setSource(QUrl::fromLocalFile(path));
                    player->play();
                    QMessageBox::information(this, "Now Playing", "Playing: " + songs.at(i)->getName());
                }
                delete songs.at(i);
            }
        }
    }
}

void ListenerWindow::deleteAccount() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        listenerRepo->remove(currentListener->getId());
        this->close();
    }
}

void ListenerWindow::searchSongs() {
    bool ok;
    QString name = QInputDialog::getText(this, "Search", "Song Name (leave empty for any):", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString genre = QInputDialog::getText(this, "Search", "Genre (leave empty for any):", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString yearStr = QInputDialog::getText(this, "Search", "Release Year (leave empty for any):", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    int year = 0;
    if (!yearStr.isEmpty()) year = yearStr.toInt();

    QList<Song*> songs = songRepo->searchSongs(name, genre, year);
    if (songs.isEmpty()) {
        QMessageBox::information(this, "Result", "No songs found matching your criteria.");
        return;
    }

    QStringList songNames;
    for (int i = 0; i < songs.size(); ++i) {
        songNames << songs.at(i)->getName() + " (" + QString::number(songs.at(i)->getReleaseYear()) + ")";
    }

    QInputDialog::getItem(this, "Search Results", "Found songs:", songNames, 0, false, &ok);
    for (int i = 0; i < songs.size(); ++i) delete songs.at(i);
}

void ListenerWindow::likeSong() {
    QList<Song*> allSongs = songRepo->getAll();
    if (allSongs.isEmpty()) {
        QMessageBox::information(this, "Info", "No songs available to like.");
        return;
    }

    QStringList songNames;
    for (int i = 0; i < allSongs.size(); ++i) {
        songNames << allSongs.at(i)->getName();
    }

    bool ok;
    QString choice = QInputDialog::getItem(this, "Like Song", "Select a song to like:", songNames, 0, false, &ok);
    if (!ok) {
        for (int i = 0; i < allSongs.size(); ++i) delete allSongs.at(i);
        return;
    }

    for (int i = 0; i < allSongs.size(); ++i) {
        if (allSongs.at(i)->getName() == choice) {
            listenerRepo->updateLiked(currentListener->getId(), allSongs.at(i)->getId(), true);
            QMessageBox::information(this, "Success", "Song added to your Favorite Songs!");
        }
        delete allSongs.at(i);
    }
}

void ListenerWindow::openChatbot() {
    ChatbotWindow* chat = new ChatbotWindow();
    chat->setAttribute(Qt::WA_DeleteOnClose);
    chat->show();
}