#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include "repositories.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow* ui;

    ArtistRepository* artistRepo;
    ListenerRepository* listenerRepo;
    SongRepository* songRepo;

    void attemptLogin();
    void attemptRegister();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};