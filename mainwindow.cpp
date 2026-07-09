#include "mainwindow.h"
#include "entities.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    artistRepo = new ArtistRepository();
    listenerRepo = new ListenerRepository();
    songRepo = new SongRepository();

    artistRepo->save(new Artist(1, "Ali Alavi", "artist1", "Bio", "1234"));
    listenerRepo->save(new Listener(2, "Reza Rezaei", "listener1", "Bio", "1234"));

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
            QMessageBox::information(this, "Success", "Welcome Artist: " + foundArtist->getUserName());
        } else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    } else if (foundListener) {
        if (foundListener->getPassword() == pass) {
            QMessageBox::information(this, "Success", "Welcome Listener: " + foundListener->getUserName());
        } else {
            QMessageBox::warning(this, "Error", "Incorrect Password.");
        }
    } else {
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

    QString userName = QInputDialog::getText(this, "Register", "Username:", QLineEdit::Normal, "", &ok);
    if (!ok || userName.isEmpty()) return;

    QString password = QInputDialog::getText(this, "Register", "Password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    if (role == "Artist") {
        artistRepo->save(new Artist(0, fullName, userName, "New Artist", password));
    } else {
        listenerRepo->save(new Listener(0, fullName, userName, "New Listener", password));
    }

    QMessageBox::information(this, "Success", "Registration successful! You can now log in.");
}