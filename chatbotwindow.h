#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ChatbotWindow : public QMainWindow {
    Q_OBJECT

private:
    QTextEdit* chatDisplay;
    QLineEdit* inputField;
    QPushButton* sendButton;
    QNetworkAccessManager* networkManager;

    void setupUI();

private slots:
    void sendQuestion();
    void receiveAnswer(QNetworkReply* reply);

public:
    ChatbotWindow(QWidget* parent = nullptr);
    ~ChatbotWindow();
};