#include "htmlserver.h"
#include "singleton.h"
#include "databasens.h"

using namespace DatabaseEnumNs;

HtmlServer::HtmlServer(QObject *parent)
    : QObject{parent},
      server_ptr{new QWebSocketServer(QStringLiteral("Html Server"), QWebSocketServer::NonSecureMode)},
      clientWrapper_ptr{new WebSocketClientWrapper(server_ptr.get())},
      channel_ptr{new QWebChannel}
{

}

void HtmlServer::startListening()
{
    if (!server_ptr->listen(QHostAddress::Any, Singleton::listenPort())) {
        qFatal("Failed to open web socket server.");
        exit(1) ;
    }
    else {
        TIME_DEBUG()<<"The server is listening on port: "<<Singleton::listenPort();
    }
    QObject::connect(clientWrapper_ptr.get(), &WebSocketClientWrapper::clientConnected,
                     channel_ptr.get(), &QWebChannel::connectTo);
    // 'core' is called by the HTML client
    channel_ptr->registerObject(QStringLiteral("core"), this);
}

void HtmlServer::loginUi()
{
    emit sendText("<div class='form-wrapper'>"
                  "<div class='icon-wrapper'>"
                      "<img src='./images/logo.png' alt='logo'/>"
                  "</div>"
                  "<div class='input-wrapper'>"
                      "<div class='border-wrapper'>"
                          "<input type='text' id='username' placeholder='用户名' class='border-item' autocomplete='off'>"
                      "</div>"
                      "<div class='border-wrapper'>"
                          "<input type='password' id='password' placeholder='密码' class='border-item' autocomplete='off'>"
                      "</div>"
                  "</div>"
                  "<div class='action'>"
                      "<div class='btn' onclick=\"(function(){core.login(document.getElementById('username').value, document.getElementById('password').value);})()\">登录</div>"
                  "</div>"
              "</div>");
}

void HtmlServer::login(const QString &adminId, const QString &password)
{
    auto db = Singleton::getInstance()->database();
    QSqlQuery sqlQuery(db);
    auto aidstr = Singleton::enumValueToKey(AdministratorInfo::adminId);
    auto pwstr = Singleton::enumValueToKey(AdministratorInfo::password);
    sqlQuery.prepare(QString("SELECT * FROM %1 WHERE %2 = ? AND %3 = ?")
                     .arg(Singleton::enumName<AdministratorInfo>(), aidstr, pwstr));
    sqlQuery.addBindValue(adminId);
    sqlQuery.addBindValue(password);
    sqlQuery.exec();
    if (sqlQuery.next()) {
        // jsonobect
        emit sendText("密码正确");
    }
    else {
        emit sendText("密码错误");
    }
}
