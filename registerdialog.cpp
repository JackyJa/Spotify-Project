#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(ArtistRepository* ar, ListenerRepository* lr, QWidget* parent) : QDialog(parent) {
    ui = new Ui::RegisterDialog;
    artistRepo = ar;
    listenerRepo = lr;
    ui->setupUi(this);
    QObject::connect(ui->signupButton, &QPushButton::clicked, this, &RegisterDialog::attemptRegister);
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

void RegisterDialog::attemptRegister() {
    QString role = ui->roleCombo->currentText();
    QString fullName = ui->fullNameInput->text().trimmed();
    QString bio = ui->bioInput->text().trimmed();
    QString userName = ui->usernameInput->text().trimmed();
    QString password = ui->passwordInput->text();

    if (fullName.isEmpty() || bio.isEmpty() || userName.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "All fields are required.");
        return;
    }

    Account* existingArtist = artistRepo->searchByUserName(userName);
    Account* existingListener = listenerRepo->searchByUserName(userName);
    if (existingArtist) delete existingArtist;
    if (existingListener) delete existingListener;

    if (existingArtist || existingListener) {
        QMessageBox::warning(this, "Error", "Username is already taken. Please choose another one.");
        return;
    }

    QRegularExpression strongRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*]).{12,}");
    QRegularExpression mediumRegex("(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9!@#$%^&*]).{6,}");

    QString strength;
    if (strongRegex.match(password).hasMatch()) strength = "Strong";
    else if (mediumRegex.match(password).hasMatch()) strength = "Medium";
    else strength = "Weak";

    if (strength == "Weak") {
        QMessageBox::warning(this, "Weak Password", "Password is too weak!\nUse at least 6 chars with upper/lower case and numbers.");
        return;
    }

    if (role == "Artist") {
        artistRepo->save(new Artist(0, fullName, userName, bio, password));
    } else {
        listenerRepo->save(new Listener(0, fullName, userName, bio, password));
    }

    QMessageBox::information(this, "Success", "Registration successful! Your password strength was: " + strength);
    this->accept();
}