#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum RequestType {
        Get = 0,
        Post,
        Put,
        Delete
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initUI();
    void handleResponse(QNetworkReply *reply);

public slots:
    void onSendRequest();

private:
    //初始化请求头部
    void initRequestHeader(QNetworkRequest &request);

    QString getRequestHeader();

private:
    QComboBox *m_typeCombobox;
    QLineEdit *m_urlEdit;
    QPlainTextEdit *m_headersEdit;
    QPlainTextEdit *m_bodyEdit;
    QTextEdit *m_responseEdit;
    QPushButton *m_sendButton;

    QNetworkAccessManager *m_networkManager;

    StoreService::Request_Pulic_Header m_systeminfo;
};
#endif // MAINWINDOW_H
