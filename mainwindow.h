#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QStringList>
#include "repositories.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    ArtistRepository* artistRepo;
    ListenerRepository* listenerRepo;
    SongRepository* songRepo;

    QWidget* loginWidget;
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QPushButton* loginButton;
    QPushButton* registerButton;

    void setupLoginUI();
    void attemptLogin();
    void attemptRegister();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};