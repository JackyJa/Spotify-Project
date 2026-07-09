#include "artistwindow.h"
#include <qlabel.h>

ArtistWindow::ArtistWindow(Account* user, ArtistRepository* aRepo, SongRepository* sRepo, QWidget* parent)
    : QMainWindow(parent), currentArtist(user), artistRepo(aRepo), songRepo(sRepo) {

    setWindowTitle("Artist Dashboard - " + currentArtist->getUserName());
    resize(500, 400);
    setupUI();
    refreshAlbums();
}

ArtistWindow::~ArtistWindow() {
    delete currentArtist;
}

void ArtistWindow::setupUI() {
    centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QLabel* welcomeLabel = new QLabel("Welcome Artist: " + currentArtist->getFullName(), centralWidget);

    albumsList = new QListWidget(centralWidget);
    btnAddAlbum = new QPushButton("Create New Album", centralWidget);
    btnAddSong = new QPushButton("Add New Song", centralWidget);
    btnEditAccount = new QPushButton("Edit Account", centralWidget);
    btnDeleteAccount = new QPushButton("Delete Account", centralWidget);

    layout->addWidget(welcomeLabel);
    layout->addWidget(new QLabel("Your Albums:"));
    layout->addWidget(albumsList);
    layout->addWidget(btnAddAlbum);
    layout->addWidget(btnAddSong);
    layout->addWidget(btnEditAccount);
    layout->addWidget(btnDeleteAccount);

    setCentralWidget(centralWidget);

    connect(btnAddAlbum, &QPushButton::clicked, this, &ArtistWindow::addAlbum);
    connect(btnAddSong, &QPushButton::clicked, this, &ArtistWindow::addSong);
    connect(btnEditAccount, &QPushButton::clicked, this, &ArtistWindow::editAccount);
    connect(btnDeleteAccount, &QPushButton::clicked, this, &ArtistWindow::deleteAccount);
}

void ArtistWindow::refreshAlbums() {
    albumsList->clear();
    albumsList->addItem("Singles");
}

void ArtistWindow::addAlbum() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Album", "Album Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QMessageBox::information(this, "Success", "Album created successfully (DB integration needed).");
        refreshAlbums();
    }
}

void ArtistWindow::addSong() {
    QStringList items;
    items << "Singles";
    bool ok;
    QString choice = QInputDialog::getItem(this, "Add Song", "Select Album:", items, 0, false, &ok);
    if (ok) {
        QString name = QInputDialog::getText(this, "Add Song", "Song Name:", QLineEdit::Normal, "", &ok);
        if (ok && !name.isEmpty()) {
            QMessageBox::information(this, "Success", "Song added successfully.");
        }
    }
}

void ArtistWindow::editAccount() {
    bool ok;
    QString newPass = QInputDialog::getText(this, "Edit Account", "New Password:", QLineEdit::Password, "", &ok);
    if (ok && !newPass.isEmpty()) {
        currentArtist->setPassword(newPass);
        QMessageBox::information(this, "Success", "Account updated.");
    }
}

void ArtistWindow::deleteAccount() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete your account?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        artistRepo->remove(currentArtist->getId());
        this->close();
    }
}