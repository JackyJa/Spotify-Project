#include "albumdetails.h"

AlbumDetailsWindow::AlbumDetailsWindow(int albumId, int artistId, SongRepository* songRepo, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Album Songs");
    resize(600, 400);

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
    for (int i = 0; i < songs.size(); ++i) {
        tableWidget->setItem(i, 0, new QTableWidgetItem(songs.at(i)->getName()));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(songs.at(i)->getReleaseYear())));
        tableWidget->setItem(i, 2, new QTableWidgetItem(songs.at(i)->getGenre()));
        tableWidget->setItem(i, 3, new QTableWidgetItem(songs.at(i)->getCoverPath()));
        delete songs.at(i);
    }

    layout->addWidget(tableWidget);
}