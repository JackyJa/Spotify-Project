#include "artistwindow.h"
#include "ui_artistwindow.h"
#include "albumdetails.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegularExpression>
#include <QFile>
#include <QPainter>
#include <QPainterPath>

ArtistWindow::ArtistWindow(Account* user, ArtistRepository* aRepo, SongRepository* sRepo, QWidget* parent) : QMainWindow(parent) {
    ui = new Ui::ArtistWindow;
    currentArtist = user;
    artistRepo = aRepo;
    songRepo = sRepo;
    ui->setupUi(this);
    albumRepo = new AlbumRepository();

    ui->welcomeLabel->setText("Welcome, " + currentArtist->getFullName() + "!");

    QString photoPath = currentArtist->getProfilePhotoPath();
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

    connect(ui->btnAddAlbum, &QPushButton::clicked, this, &ArtistWindow::addAlbum);
    connect(ui->btnAddSong, &QPushButton::clicked, this, &ArtistWindow::addSong);
    connect(ui->btnEditAccount, &QPushButton::clicked, this, &ArtistWindow::editAccount);
    connect(ui->btnDeleteAccount, &QPushButton::clicked, this, &ArtistWindow::deleteAccount);
    connect(ui->albumsList, &QListWidget::itemClicked, this, &ArtistWindow::viewAlbumSongs);
    connect(ui->btnSignOut, &QPushButton::clicked, this, &ArtistWindow::signOut);

    refreshAlbums();
}

ArtistWindow::~ArtistWindow() {
    delete ui;
    delete currentArtist;
    delete albumRepo;
}

void ArtistWindow::refreshAlbums() {
    ui->albumsList->clear();
    ui->albumsList->addItem("Singles");

    QList<Album*> myAlbums = albumRepo->albums(currentArtist->getId());
    for (int i = 0; i < myAlbums.size(); i++) {
        ui->albumsList->addItem(myAlbums[i]->getName());
        delete myAlbums[i];
    }
}

void ArtistWindow::addAlbum() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Album", "Album Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString coverPath = QFileDialog::getOpenFileName(this, "Select Album Cover", "", "Image Files (*.jpg *.png *.jpeg)");
    if (coverPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Album cover is required.");
        return;
    }

    albumRepo->save(new Album(0, name, currentArtist->getId(), coverPath));
    QMessageBox::information(this, "Success", "Album created successfully.");
    refreshAlbums();
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

    QString yearStr = QInputDialog::getText(this, "Add Song", "Release Year (e.g. 2023):", QLineEdit::Normal, "", &ok);
    if (!ok || yearStr.isEmpty()) return;
    int year = yearStr.toInt();

    QString genre = QInputDialog::getText(this, "Add Song", "Genre (e.g. Pop, Rock):", QLineEdit::Normal, "", &ok);
    if (!ok || genre.isEmpty()) return;

    QString filePath = QFileDialog::getOpenFileName(this, "Select Audio File", "", "Audio Files (*.mp3 *.wav)");
    if (filePath.isEmpty()) return;

    QString coverPath = QFileDialog::getOpenFileName(this, "Select Song Cover", "", "Image Files (*.jpg *.png *.jpeg)");
    if (coverPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Song cover is required.");
        return;
    }

    int selectedAlbumId = 0;
    if (choice != "Singles (No Album)") {
        for (int i = 0; i < myAlbums.size(); i++) {
            if (myAlbums[i]->getName() == choice) {
                selectedAlbumId = myAlbums[i]->getId();
                break;
            }
        }
    }
    songRepo->save(new Song(0, name, year, genre, filePath, currentArtist->getId(), selectedAlbumId, coverPath));
    QMessageBox::information(this, "Success", "Song added successfully.");
}

void ArtistWindow::editAccount() {
    bool ok;
    QString fullName = QInputDialog::getText(this, "Edit", "Full Name:", QLineEdit::Normal, currentArtist->getFullName(), &ok);
    if (!ok || fullName.isEmpty()) return;
    QString userName = QInputDialog::getText(this, "Edit", "Username:", QLineEdit::Normal, currentArtist->getUserName(), &ok);
    if (!ok || userName.isEmpty()) return;
    QString bio = QInputDialog::getText(this, "Edit", "Biography:", QLineEdit::Normal, currentArtist->getBiography(), &ok);
    if (!ok || bio.isEmpty()) return;
    QString password = QInputDialog::getText(this, "Edit", "New Password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QRegularExpression strongRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*]).{12,}");
    QRegularExpression mediumRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9!@#$%^&*]).{6,}");
    QString strength;
    if (strongRegex.match(password).hasMatch()) strength = "Strong";
    else if (mediumRegex.match(password).hasMatch()) strength = "Medium";
    else strength = "Weak";

    if (strength == "Weak") {
        QMessageBox::warning(this, "Error", "Password too weak.");
        return;
    }

    currentArtist->setFullName(fullName);
    currentArtist->setUserName(userName);
    currentArtist->setBiography(bio);
    currentArtist->setPassword(password);
    if (artistRepo->update(currentArtist)) QMessageBox::information(this, "Success", "Account updated.");
    else QMessageBox::warning(this, "Error", "Update failed.");
}

void ArtistWindow::deleteAccount() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete", "Are you sure?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        artistRepo->remove(currentArtist->getId());
        this->signOut();
    }
}

void ArtistWindow::viewAlbumSongs(QListWidgetItem* item) {
    QString selected = item->text();
    int albumId = 0;

    if (selected != "Singles") {
        QList<Album*> myAlbums = albumRepo->albums(currentArtist->getId());
        for (int i = 0; i < myAlbums.size(); i++) {
            if (myAlbums[i]->getName() == selected) albumId = myAlbums[i]->getId();
            delete myAlbums[i];
        }
        if (albumId == 0) return;
    }

    AlbumDetailsWindow* details = new AlbumDetailsWindow(albumId, currentArtist->getId(), songRepo);
    details->setAttribute(Qt::WA_DeleteOnClose);
    details->exec();
}

void ArtistWindow::signOut() {
    MainWindow* login = new MainWindow();
    login->show();
    this->close();
}