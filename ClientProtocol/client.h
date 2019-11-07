#ifndef CLIENT_H
#define CLIENT_H

#include "basenode.h"

namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT Client: protected BaseNode
{
    Q_OBJECT
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString lastMessage READ lastMessage WRITE setLastMessage NOTIFY lastMessageChanged)

public:
    enum Status {
        Offline,
        Online,
        Logined
    };

    explicit Client(const QHostAddress& address, unsigned short port);
    void connectClient();
    void setHost(const QHostAddress& address, unsigned short port);
    bool login(const QString& userMail, const QByteArray& rawPath);

    Q_INVOKABLE int status() const;
    Q_INVOKABLE QString lastMessage() const;

private slots:
    void handleIncomingData(Package pkg, const QHostAddress &sender);
    void setLastMessage(QString lastMessage);

private:
    Status _status = Offline;
    QHostAddress _address;
    unsigned short _port;

    QString _lastMessage;

signals:
    void statusChanged(int status);

    void lastMessageChanged(QString lastMessage);
};

}

#endif // CLIENT_H
