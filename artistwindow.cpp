#include "artistwindow.h"
#include "repositories.h"
#include <QLabel>
#include <QRegularExpression>
#include <QFileDialog>

ArtistWindow::ArtistWindow(Account* user, ArtistRepository* aRepo, SongRepository* sRepo, QWidget* parent)
    : QMainWindow(parent) {
    currentArtist = user;
    artistRepo = aRepo;
    songRepo = sRepo;
    albumRepo = new AlbumRepository();
    setWindowTitle("Artist Dashboard - " + currentArtist->getUserName());
    resize(500, 400);
    setupUI();
    refreshAlbums();
}

ArtistWindow::~ArtistWindow() {
    delete currentArtist;
    delete albumRepo;
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

    QList<Album*> myAlbums = albumRepo->albums(currentArtist->getId());
    for (int i = 0; i < myAlbums.size(); i++) {
        albumsList->addItem(myAlbums[i]->getName());
    }
}

void ArtistWindow::addAlbum() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Album", "Album Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        albumRepo->save(new Album(0, name, currentArtist->getId(), ""));
        QMessageBox::information(this, "Success", "Album created successfully in database.");
        refreshAlbums();
    }
}

void ArtistWindow::addSong() {
    QList<Album*> myAlbums = albumRepo->albums(currentArtist->getId());
    QStringList items;
    items << "Singles (No Album)";
    for (int i = 0; i < myAlbums.size(); i++) {
        items << myAlbums[i]->getName();
    }

    bool ok;
    QString choice = QInputDialog::getItem(this, "Add Song", "Select Album:", items, 0, false, &ok);
    if (!ok) return;

    QString name = QInputDialog::getText(this, "Add Song", "Song Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", "", "Audio Files (*.mp3 *.wav)");
    if (filePath.isEmpty()) return;

    int selectedAlbumId = 0;
    if (choice != "Singles (No Album)") {
        for (int i = 0; i < myAlbums.size(); i++) {
            if (myAlbums.at(i)->getName() == choice) {
                selectedAlbumId = myAlbums[i]->getId();
                break;
            }
        }
    }
    songRepo->save(new Song(0, name, 0, "", filePath, currentArtist->getId(), selectedAlbumId));
    QMessageBox::information(this, "Success", "Song added successfully in database.");
}

void ArtistWindow::editAccount() {
    bool ok;
    QString fullName = QInputDialog::getText(this, "Edit Account", "Full Name:", QLineEdit::Normal, currentArtist->getFullName(), &ok);
    if (!ok || fullName.isEmpty()) return;

    QString userName = QInputDialog::getText(this, "Edit Account", "Username:", QLineEdit::Normal, currentArtist->getUserName(), &ok);
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

    currentArtist->setFullName(fullName);
    currentArtist->setUserName(userName);
    currentArtist->setPassword(password);

    artistRepo->update(currentArtist);
    QMessageBox::information(this, "Success", "Account updated in database.");
}

void ArtistWindow::deleteAccount() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete your account?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        artistRepo->remove(currentArtist->getId());
        this->close();
    }
}