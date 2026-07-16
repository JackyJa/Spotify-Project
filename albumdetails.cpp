#include "albumdetails.h"
#include <QFile>
#include <QListWidgetItem>
#include <QPainter>
#include <QPainterPath>

QPixmap makeRoundedPixmapAlbum(const QString& path, int size) {
    QPixmap src(path);
    QPixmap roundedPix(size, size);
    roundedPix.fill(Qt::transparent);
    QPainter painter(&roundedPix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath roundPath;
    roundPath.addRoundedRect(0, 0, size, size, 10, 10);
    painter.setClipPath(roundPath);
    painter.drawPixmap(0, 0, src.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    return roundedPix;
}

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
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"Cover", "Name", "Release Year", "Genre", "Audio Path"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setIconSize(QSize(50, 50));
    tableWidget->verticalHeader()->setDefaultSectionSize(60);

    QList<Song*> songs;
    if (albumId == 0) {
        setWindowTitle("Singles");
        songs = songRepo->singleSongs(artistId);
    } else {
        songs = songRepo->getByAlbum(albumId);
    }

    tableWidget->setRowCount(songs.size());
    for (int i = 0; i < songs.size(); i++) {

        QTableWidgetItem* coverItem = new QTableWidgetItem();
        QString coverPath = songs[i]->getCoverPath();
        if (!coverPath.isEmpty() && QFile::exists(coverPath)) {
            coverItem->setData(Qt::DecorationRole, makeRoundedPixmapAlbum(coverPath, 50));
        }
        tableWidget->setItem(i, 0, coverItem);


        tableWidget->setItem(i, 1, new QTableWidgetItem(songs[i]->getName()));
        tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(songs[i]->getReleaseYear())));
        tableWidget->setItem(i, 3, new QTableWidgetItem(songs[i]->getGenre()));
        tableWidget->setItem(i, 4, new QTableWidgetItem(songs[i]->getAudioFilePath()));
        delete songs[i];
    }

    layout->addWidget(tableWidget);
}