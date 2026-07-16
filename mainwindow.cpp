#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "artistwindow.h"
#include "listenerwindow.h"
#include "registerdialog.h"
#include <QMovie>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui = new Ui::MainWindow;
    artistRepo = new ArtistRepository();
    listenerRepo = new ListenerRepository();
    songRepo = new SongRepository();

    ui->setupUi(this);


    QMovie* movie = new QMovie("A:/Qt_Ptojects/Project/Spotify.gif");
    movie->setScaledSize(QSize(150, 150));
    ui->logoLabel->setMovie(movie);
    movie->start();


    QObject::connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::attemptLogin);
    QObject::connect(ui->registerButton, &QPushButton::clicked, this, &MainWindow::attemptRegister);
}

MainWindow::~MainWindow() {
    delete ui;
    delete artistRepo;
    delete listenerRepo;
    delete songRepo;
}

void MainWindow::attemptLogin() {
    QString user = ui->usernameInput->text();
    QString pass = ui->passwordInput->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and Password cannot be empty.");
        return;
    }

    Account* foundArtist = artistRepo->searchByUserName(user);
    Account* foundListener = listenerRepo->searchByUserName(user);

    if (foundArtist) {
        if (foundArtist->getPassword() == pass) {
            this->deleteLater();
            ArtistWindow* aw = new ArtistWindow(foundArtist, artistRepo, songRepo);
            aw->show();
        }
        else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    }
    else if (foundListener) {
        if (foundListener->getPassword() == pass) {
            this->deleteLater();
            ListenerWindow* lw = new ListenerWindow(foundListener, listenerRepo, songRepo);
            lw->show();
        }
        else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    }
    else {
        QMessageBox::warning(this, "Error", "User not found.");
    }
}

void MainWindow::attemptRegister() {
    RegisterDialog regDialog(artistRepo, listenerRepo, this);
    regDialog.exec();
}