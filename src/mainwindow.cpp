#include "mainwindow.h"
#include "utils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>

const QString DefaultURL = QStringLiteral("https://appstore.uniontech.com/store-dist-operaton/getNavInfo");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    Utils::getSystemInfo(m_systeminfo);
    initUI();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // URL输入行
    QHBoxLayout *urlLayout = new QHBoxLayout();
    m_typeCombobox = new QComboBox();
    m_typeCombobox->setMinimumWidth(100);
    m_typeCombobox->addItem("Get", RequestType::Get);
    m_typeCombobox->addItem("Post", RequestType::Post);
    m_typeCombobox->addItem("Put", RequestType::Put);
    m_typeCombobox->addItem("Delete", RequestType::Delete);
    m_urlEdit = new QLineEdit(DefaultURL);
    m_sendButton = new QPushButton("Send");
    urlLayout->addWidget(m_typeCombobox);
    urlLayout->addWidget(m_urlEdit);
    urlLayout->addWidget(m_sendButton);

    // 请求界面
    m_headersEdit = new QPlainTextEdit();
    m_headersEdit->setPlaceholderText("Enter headers (Format: Key: Value)");
    m_headersEdit->setPlainText(getRequestHeader());

    m_bodyEdit = new QPlainTextEdit();
    m_bodyEdit->setPlaceholderText("Enter body");

    QTabWidget *requestTabWidget = new QTabWidget(centralWidget);
    requestTabWidget->addTab(m_headersEdit, "Request Headers");
    requestTabWidget->addTab(m_bodyEdit, "Request Body");

    // 响应显示
    QWidget *responseWidget = new QWidget(centralWidget);
    QVBoxLayout *responseLayout = new QVBoxLayout(responseWidget);
    m_responseEdit = new QTextEdit();
    m_responseEdit->setReadOnly(true);
    responseLayout->addWidget(new QLabel("Response:"));
    responseLayout->addWidget(m_responseEdit);

    QSplitter *bodySplitter = new QSplitter();
    bodySplitter->addWidget(requestTabWidget);
    bodySplitter->addWidget(responseWidget);

    mainLayout->addLayout(urlLayout);
    mainLayout->addWidget(bodySplitter);

    m_urlEdit->setFocus();
    setCentralWidget(centralWidget);
    resize(800, 600);

    // 连接信号槽
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendRequest);
}

void MainWindow::handleResponse(QNetworkReply *reply)
{
    qInfo() << "Response received" << reply->error();
    QString responseText;

    // 处理错误
    if (reply->error() != QNetworkReply::NoError) {
        responseText = QString("Error: %1\n%2").arg(reply->errorString()).arg(reply->readAll());
    } else {
        // 构建响应信息
        responseText += QString("Status: %1\n").arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());

        // 响应头
        responseText += "\nHeaders:\n";
        foreach (const QNetworkReply::RawHeaderPair &pair, reply->rawHeaderPairs()) {
            responseText += QString("%1: %2\n").arg(QString(pair.first)).arg(QString(pair.second));
        }

        // 响应体
        responseText += "\nBody:\n";
        responseText += QString::fromUtf8(reply->readAll());
    }

    m_responseEdit->setText(responseText);

    reply->close();
    reply->deleteLater();
    reply = nullptr;

    m_sendButton->setEnabled(true);
}

void MainWindow::onSendRequest()
{
    m_responseEdit->clear();
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty())
        return;

    m_sendButton->setEnabled(false);

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 初始化请求头
    initRequestHeader(request);

    // 获取body数据
    QString bodyText = m_bodyEdit->toPlainText();
    QByteArray data = bodyText.toLatin1();

    // 发送请求
    QNetworkReply *reply;
    RequestType type = m_typeCombobox->currentData().value<RequestType>();
    switch (type) {
    case Get:
        reply = m_networkManager->get(request);
        break;
    case Post:
        reply = m_networkManager->post(request, data);
        break;
    case Put:
        reply = m_networkManager->put(request, data);
        break;
    case Delete:
        reply = m_networkManager->deleteResource(request);
        break;
    default:
        break;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    });
}

void MainWindow::initRequestHeader(QNetworkRequest &request)
{
    QString headersText = m_headersEdit->toPlainText();
    QStringList headers = headersText.split('\n', Qt::SkipEmptyParts);
    for (const QString &header : headers) {
        int colonPos = header.indexOf(':');
        if (colonPos > 0) {
            QString key = header.left(colonPos).trimmed();
            QString value = header.mid(colonPos + 1).trimmed();
            request.setRawHeader(key.toUtf8(), value.toUtf8());
        }
    }
}

QString MainWindow::getRequestHeader()
{
    QString headersStr;
    headersStr += QString("arch: %1\n").arg(m_systeminfo.arch);
    headersStr += QString("iswayland: %1\n").arg(m_systeminfo.iswayland);
    headersStr += QString("mode: %1\n").arg(m_systeminfo.mode);
    headersStr += QString("platform: %1\n").arg(m_systeminfo.platform);
    headersStr += QString("region: %1\n").arg(m_systeminfo.region);
    headersStr += QString("language: %1\n").arg(m_systeminfo.language);
    headersStr += QString("baseline_version: %1\n").arg(m_systeminfo.baseline_version);
    headersStr += QString("baseline_main_version: %1\n").arg(m_systeminfo.baseline_main_version);
    headersStr += QString("os_build: %1\n").arg(m_systeminfo.os_build);
    headersStr += QString("client_version: %1\n").arg(m_systeminfo.client_version);
    headersStr += QString("eabi: %1\n").arg(m_systeminfo.eabi);
    headersStr += QString("mac: %1\n").arg(m_systeminfo.mac);
    headersStr += QString("token: %1\n").arg(m_systeminfo.token);
    headersStr += QString("referer: %1\n").arg(m_systeminfo.referer);
    headersStr += QString("motherboard: %1\n").arg(m_systeminfo.motherboard);
    headersStr += QString("cpu_clip: %1\n").arg(m_systeminfo.cpu_clip);
    headersStr += QString("is_system_active: %1\n").arg(Utils::getAuthorizationState());
    headersStr += QString("supfeatures: %1\n").arg(m_systeminfo.supFeatures);
    headersStr += QString("templateId: %1\n").arg(m_systeminfo.templateId);
    headersStr += QString("User-Agent: %1").arg(m_systeminfo.user_agent.toUserAgentStr());    
    return headersStr;
}
