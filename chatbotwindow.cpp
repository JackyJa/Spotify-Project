#include "chatbotwindow.h"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QLabel>

ChatbotWindow::ChatbotWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Spotify AI Assistant");
    resize(400, 500);

    this->setStyleSheet("QMainWindow { background-color: #121212; }"
                        "QLabel { color: #FFFFFF; font-weight: bold; }"
                        "QTextEdit { background-color: #181818; color: #FFFFFF; border: 2px solid #282828; border-radius: 8px; padding: 10px; font-size: 14px; }"
                        "QLineEdit { background-color: #282828; color: #FFFFFF; border: 2px solid #535353; border-radius: 15px; padding: 10px; font-size: 14px; }"
                        "QLineEdit:focus { border: 2px solid #1DB954; }"
                        "QPushButton { background-color: #1DB954; color: #000000; border: none; border-radius: 15px; padding: 10px; font-weight: bold; }"
                        "QPushButton:hover { background-color: #1ed760; }");
    networkManager = new QNetworkAccessManager(this);
    setupUI();

    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &ChatbotWindow::receiveAnswer);
}

ChatbotWindow::~ChatbotWindow() {}

void ChatbotWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    chatDisplay = new QTextEdit(centralWidget);
    chatDisplay->setReadOnly(true);
    chatDisplay->append("AI: سلام! من دستیار اسپاتیفای شما هستم. سوالی درباره آهنگ‌ها، آلبوم‌ها یا خواننده‌ها بپرسید.");

    inputField = new QLineEdit(centralWidget);
    inputField->setPlaceholderText("سوال خود را اینجا بنویسید...");

    sendButton = new QPushButton("Ask", centralWidget);

    layout->addWidget(new QLabel("Chat History:"));
    layout->addWidget(chatDisplay);
    layout->addWidget(inputField);
    layout->addWidget(sendButton);

    setCentralWidget(centralWidget);

    QObject::connect(sendButton, &QPushButton::clicked, this, &ChatbotWindow::sendQuestion);
    QObject::connect(inputField, &QLineEdit::returnPressed, this, &ChatbotWindow::sendQuestion);
}

void ChatbotWindow::sendQuestion() {
    QString question = inputField->text().trimmed();
    if (question.isEmpty()) return;

    chatDisplay->append("You: " + question);
    inputField->clear();

    QNetworkRequest request(QUrl("http://127.0.0.1:8000/ask"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["question"] = question;

    networkManager->post(request, QJsonDocument(json).toJson());
}

void ChatbotWindow::receiveAnswer(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QString answer = doc.object().value("answer").toString();
        chatDisplay->append("AI: " + answer);
    }
    else {
        chatDisplay->append("AI: Error connecting to server. Make sure Python API is running.");
    }
    reply->deleteLater();
}