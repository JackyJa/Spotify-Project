#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    a.setStyleSheet(
        "QMainWindow, QDialog { background-color: #121212; }"
        "QLabel { color: #FFFFFF; font-size: 14px; font-weight: bold; }"
        "QLineEdit { background-color: #282828; color: #FFFFFF; border: 2px solid #535353; border-radius: 8px; padding: 10px; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #1DB954; }"
        "QPushButton { background-color: #1DB954; color: #000000; border: none; border-radius: 15px; padding: 12px; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1ed760; }"
        "QPushButton:pressed { background-color: #169c46; }"
        "QListWidget { background-color: #282828; color: #FFFFFF; border: none; border-radius: 8px; font-size: 14px; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #3e3e3e; }"
        "QListWidget::item:selected { background-color: #1DB954; color: #000000; font-weight: bold; }"
        "QTableWidget { background-color: #282828; color: #FFFFFF; gridline-color: #3e3e3e; border: none; }"
        "QHeaderView::section { background-color: #121212; color: #1DB954; padding: 8px; border: none; font-weight: bold; }"
        );

    if (!DatabaseManager::init()) {
        QMessageBox::critical(nullptr, "Error", "Could not open database!");
        return -1;
    }

    MainWindow* w = new MainWindow();
    w->show();
    return a.exec();
}