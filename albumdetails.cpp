#include "albumdetails.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
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
    songRepo = songRepo;
    currentAlbumId = albumId;
    currentArtistId = artistId;
    setWindowTitle("Album Songs");
    resize(700, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"Cover", "Name", "Year", "Genre", "Audio Path"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setIconSize(QSize(50, 50));
    tableWidget->verticalHeader()->setDefaultSectionSize(60);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setStyleSheet("QTableWidget { background-color: #181818; color: #FFFFFF; border: none; font-size: 14px; }"
                               "QTableWidget::item { padding: 10px; border-bottom: 1px solid #282828; }"
                               "QTableWidget::item:selected { background-color: #1DB954; color: #000000; }"
                               "QHeaderView::section { background-color: #000000; color: #1DB954; padding: 10px; border: none; font-weight: bold; font-size: 16px; }");

    QPushButton* btnEdit = new QPushButton("Edit Selected Song", this);
    btnEdit->setStyleSheet("background-color: #535353; color: white; border-radius: 15px; padding: 10px; font-weight: bold;");

    QPushButton* btnDelete = new QPushButton("Delete Selected Song", this);
    btnDelete->setStyleSheet("background-color: #ff4c4c; color: white; border-radius: 15px; padding: 10px; font-weight: bold;");

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDelete);

    layout->addWidget(tableWidget);
    layout->addLayout(btnLayout);

    connect(btnEdit, &QPushButton::clicked, this, &AlbumDetailsWindow::onEditSongClicked);
    connect(btnDelete, &QPushButton::clicked, this, &AlbumDetailsWindow::onDeleteSongClicked);

    refreshTable();
}

AlbumDetailsWindow::~AlbumDetailsWindow() {
    for (int i = 0; i < songs.size(); i++) {
        delete songs[i];
    }
}

void AlbumDetailsWindow::refreshTable() {

    for (int i = 0; i < songs.size(); i++){
        delete songs[i];
    }
    songs.clear();
    tableWidget->setRowCount(0);

    if (currentAlbumId == 0) {
        setWindowTitle("Singles");
        songs = songRepo->singleSongs(currentArtistId);
    }
    else {
        songs = songRepo->getByAlbum(currentAlbumId);
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
    }
}

void AlbumDetailsWindow::onEditSongClicked() {
    int row = tableWidget->currentRow();
    if (row < 0 || row >= songs.size()) {
        QMessageBox::warning(this, "Error", "Please select a song first.");
        return;
    }

    Song* target = songs[row];
    bool ok;

    QString newName = QInputDialog::getText(this, "Edit Song", "Name:", QLineEdit::Normal, target->getName(), &ok);
    if (!ok || newName.isEmpty()) return;

    QString yearStr = QInputDialog::getText(this, "Edit Song", "Year:", QLineEdit::Normal, QString::number(target->getReleaseYear()), &ok);
    if (!ok || yearStr.isEmpty()) return;
    int newYear = yearStr.toInt();

    QString newGenre = QInputDialog::getText(this, "Edit Song", "Genre:", QLineEdit::Normal, target->getGenre(), &ok);
    if (!ok || newGenre.isEmpty()) return;

    QString newCoverPath = QFileDialog::getOpenFileName(this, "Select New Cover (Leave empty to keep current)", "", "Image Files (*.jpg *.png *.jpeg)");
    if (newCoverPath.isEmpty()) newCoverPath = target->getCoverPath();

    target->setName(newName);
    target->setReleaseYear(newYear);
    target->setGenre(newGenre);
    target->setCoverPath(newCoverPath);

    if (songRepo->update(target)) {
        QMessageBox::information(this, "Success", "Song updated.");
        refreshTable();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to update song.");
    }
}

void AlbumDetailsWindow::onDeleteSongClicked() {
    int row = tableWidget->currentRow();
    if (row < 0 || row >= songs.size()) {
        QMessageBox::warning(this, "Error", "Please select a song first.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this song permanently?", QMessageBox::Yes|QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    Song* target = songs[row];
    songRepo->remove(target->getId());

    QMessageBox::information(this, "Success", "Song deleted successfully.");
    refreshTable();
}