#include "albumdetails.h"

AlbumDetailsWindow::AlbumDetailsWindow(int albumId, int artistId, SongRepository* songRepo, QWidget* parent) : QDialog(parent) {

    setWindowTitle("Album Songs");
    resize(600, 400);
    this->setStyleSheet("QDialog { background-color: #121212; }"
                        "QTableWidget { background-color: #181818; color: #FFFFFF; border: none; font-size: 14px; }"
                        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #282828; }"
                        "QTableWidget::item:selected { background-color: #1DB954; color: #000000; }"
                        "QHeaderView::section { background-color: #000000; color: #1DB954; padding: 10px; border: none; font-weight: bold; font-size: 16px; }");

    QVBoxLayout* layout = new QVBoxLayout(this);
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Name", "Release Year", "Genre", "Cover Path"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QList<Song*> songs;
    if (albumId == 0) {
        setWindowTitle("Singles");
        songs = songRepo->singleSongs(artistId);
    } else {
        songs = songRepo->getByAlbum(albumId);
    }

    tableWidget->setRowCount(songs.size());
    for (int i = 0; i < songs.size(); i++) {
        tableWidget->setItem(i, 0, new QTableWidgetItem(songs.at(i)->getName()));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(songs[i]->getReleaseYear())));
        tableWidget->setItem(i, 2, new QTableWidgetItem(songs[i]->getGenre()));
        tableWidget->setItem(i, 3, new QTableWidgetItem(songs[i]->getCoverPath()));
        delete songs[i];
    }

    layout->addWidget(tableWidget);
}