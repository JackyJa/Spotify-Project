#include "listenerwindow.h"
#include <qlabel.h>
#include <QRegularExpression>

ListenerWindow::ListenerWindow(Account* user, ListenerRepository* lRepo, SongRepository* sRepo, QWidget* parent)
    : QMainWindow(parent), currentListener(user), listenerRepo(lRepo), songRepo(sRepo) {

    playlistRepo = new PlaylistRepository();
    setWindowTitle("Listener Dashboard - " + currentListener->getUserName());
    resize(500, 400);
    setupUI();
    refreshPlaylists();
}

ListenerWindow::~ListenerWindow() {
    delete currentListener;
    delete playlistRepo;
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

    layout->addWidget(welcomeLabel);
    layout->addWidget(new QLabel("Your Playlists:"));
    layout->addWidget(playlistsList);
    layout->addWidget(btnCreatePlaylist);
    layout->addWidget(btnViewArtists);
    layout->addWidget(btnEditAccount);
    layout->addWidget(btnDeleteAccount);

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
    QMessageBox::information(this, "Info", "Here you will see a list of all artists.");
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

void ListenerWindow::deleteAccount() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        listenerRepo->remove(currentListener->getId());
        this->close();
    }
}