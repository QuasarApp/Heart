#ifndef CLIENT_H
#define CLIENT_H

#include "basenode.h"
#include "ratingusernode.h"

namespace NP {


class NETWORKPROTOCOLSHARED_EXPORT Client: public RatingUserNode
{
    Q_OBJECT
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString lastMessage READ lastMessage WRITE setLastMessage NOTIFY lastMessageChanged)

public:
    enum Status {
        Offline,
        Online,
        Logined
    };

    explicit Client();
    explicit Client(const QHostAddress& address, unsigned short port);
    explicit Client(const QString& address, unsigned short port);

    void connectClient();

    void setHost(const QString &address, unsigned short port);
    void setHost(const QHostAddress& address, unsigned short port);

    bool login(const QString& userMail, const QByteArray& rawPath);
    bool logout();
    bool syncUserData();

    Q_INVOKABLE int status() const;
    Q_INVOKABLE QString lastMessage() const;

protected:
    bool registerSocket(QAbstractSocket *socket, const QHostAddress* clientAddress) override;

private slots:
    void handleIncomingData(SP<AbstractData> obj, const QHostAddress &);
    void setLastMessage(QString lastMessage);
    void socketStateChanged(QAbstractSocket::SocketState);

private:
    Status _status = Offline;
    QString _domain;
    QHostAddress _address;
    unsigned short _port;

    QString _lastMessage;
    SP<UserData> _user;

    void setStatus(Status);

//    void handleNewSocketRegistered(QWeakPointer<AbstractNodeInfo> info);
signals:
    void statusChanged(int status);

    void lastMessageChanged(QString lastMessage);
};

}

#endif // CLIENT_H
