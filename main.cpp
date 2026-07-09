#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    if (!DatabaseManager::init()) {
        QMessageBox::critical(nullptr, "Error", "Could not open database!");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}