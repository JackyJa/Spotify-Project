#include "mainwindow.h"
#include "artistwindow.h"
#include "listenerwindow.h"
#include "entities.h"
#include <QRegularExpression>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    artistRepo = new ArtistRepository();
    listenerRepo = new ListenerRepository();
    songRepo = new SongRepository();

    setupLoginUI();
    setWindowTitle("Spotify Music");
    resize(400, 300);
}

MainWindow::~MainWindow() {
    delete artistRepo;
    delete listenerRepo;
    delete songRepo;
}

void MainWindow::setupLoginUI() {
    loginWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(loginWidget);

    QLabel* titleLabel = new QLabel("Login to Spotify", loginWidget);
    titleLabel->setAlignment(Qt::AlignCenter);

    usernameInput = new QLineEdit(loginWidget);
    usernameInput->setPlaceholderText("Username");

    passwordInput = new QLineEdit(loginWidget);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", loginWidget);
    registerButton = new QPushButton("Register", loginWidget);

    layout->addWidget(titleLabel);
    layout->addWidget(usernameInput);
    layout->addWidget(passwordInput);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    setCentralWidget(loginWidget);

    QObject::connect(loginButton, &QPushButton::clicked, this, &MainWindow::attemptLogin);
    QObject::connect(registerButton, &QPushButton::clicked, this, &MainWindow::attemptRegister);
}

void MainWindow::attemptLogin() {
    QString user = usernameInput->text();
    QString pass = passwordInput->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and Password cannot be empty.");
        return;
    }

    Account* foundArtist = artistRepo->searchByUserName(user);
    Account* foundListener = listenerRepo->searchByUserName(user);

    if (foundArtist) {
        if (foundArtist->getPassword() == pass) {
            this->hide();
            ArtistWindow* aw = new ArtistWindow(foundArtist, artistRepo, songRepo);
            aw->show();
        } else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    }
    else if (foundListener) {
        if (foundListener->getPassword() == pass) {
            this->hide();
            ListenerWindow* lw = new ListenerWindow(foundListener, listenerRepo, songRepo);
            lw->show();
        } else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    }
    else {
        QMessageBox::warning(this, "Error", "User not found.");
    }
}

void MainWindow::attemptRegister() {
    QStringList roles;
    roles << "Artist" << "Listener";

    bool ok;
    QString role = QInputDialog::getItem(this, "Register", "Select Role:", roles, 0, false, &ok);
    if (!ok || role.isEmpty()) return;

    QString fullName = QInputDialog::getText(this, "Register", "Full Name:", QLineEdit::Normal, "", &ok);
    if (!ok || fullName.isEmpty()) return;


    QString userName;
    while (true) {
        userName = QInputDialog::getText(this, "Register", "Choose a Username:", QLineEdit::Normal, "", &ok);
        if (!ok || userName.isEmpty()) return;

        Account* existingArtist = artistRepo->searchByUserName(userName);
        Account* existingListener = listenerRepo->searchByUserName(userName);


        if (existingArtist) delete existingArtist;
        if (existingListener) delete existingListener;

        if (existingArtist || existingListener) {
            QMessageBox::warning(this, "Error", "Username is already taken. Please choose another one.");
        } else {
            break;
        }
    }


    QString password;
    QRegularExpression strongRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*]).{12,}");
    QRegularExpression mediumRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9!@#$%^&*]).{6,}");

    while (true) {
        password = QInputDialog::getText(this, "Register", "Choose a Password:", QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;

        QString strength;
        if (strongRegex.match(password).hasMatch()) strength = "Strong";
        else if (mediumRegex.match(password).hasMatch()) strength = "Medium";
        else strength = "Weak";

        if (strength == "Weak") {
            QMessageBox::warning(this, "Weak Password", "Password is too weak!\nUse at least 6 chars with upper/lower case and numbers.");
        } else {
            QMessageBox::information(this, "Password Strength", "Your password strength is: " + strength);
            break;
        }
    }


    if (role == "Artist") {
        artistRepo->save(new Artist(0, fullName, userName, "New Artist", password));
    } else {
        listenerRepo->save(new Listener(0, fullName, userName, "New Listener", password));
    }

    QMessageBox::information(this, "Success", "Registration successful! You can now log in.");
}