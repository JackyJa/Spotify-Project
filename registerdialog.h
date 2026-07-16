#pragma once

#include <QDialog>
#include "repositories.h"


namespace Ui {
    class RegisterDialog;
}

class RegisterDialog : public QDialog {
    Q_OBJECT

private:
    Ui::RegisterDialog* ui;
    ArtistRepository* artistRepo;
    ListenerRepository* listenerRepo;
    QString selectedPhotoPath;

private slots:
    void attemptRegister();
    void onSelectPhotoClicked();

public:
    RegisterDialog(ArtistRepository* ar, ListenerRepository* lr, QWidget* parent = nullptr);
    ~RegisterDialog();
};