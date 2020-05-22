/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CLIENT_H
#define CLIENT_H

#include "basenode.h"
#include "ratingusernode.h"

namespace NP {

/**
 * @brief The Client class - this is cleent inplementation of RegiserUserNode. This class support qml
 */
class NETWORKPROTOCOLSHARED_EXPORT Client: public RatingUserNode
{
    Q_OBJECT
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString lastMessage READ lastMessage WRITE setLastMessage NOTIFY lastMessageChanged)

public:

    /**
     * @brief The Status enum
     */
    enum Status {
        /// node if offline.
        Offline,

        /// node is connected to host
        Online,

        /// node logined seccussful
        Logined
    };

    /**
     * @brief Client
     */
    explicit Client();

    /**
     * @brief Client - this constructor invoke a setHost method.
     * @param address - ip address of remote host
     * @param port - port of remote host
     */
    explicit Client(const QHostAddress& address, unsigned short port);

    /**
     * @brief Client - this constructor invoke a setHost method.
     * @param domain - domain of remote host
     * @param port port of remote host
     */
    explicit Client(const QString& domain, unsigned short port);

    /**
     * @brief connectClient - try connect to remote host uses information after call setHost method.
     * @note if you use setHost for domain then connection attempt will be made after receiving the address from the dns server
     *  else the connection will be established instantly.
     */
    void connectClient();

    /**
     * @brief setHost - this function set domain name of remote host, and after invoke the connectClient method try find a ip address on DNS server.
     * @param domain - the domain address
     * @param port - the port of remote host
     */
    void setHost(const QString &domain, unsigned short port);

    /**
     * @brief setHost - this function set ip address of remote host, and do not requarement DNS.
     * @param address - the ip address of remote host
     * @param por - the port of remote host.
     */
    void setHost(const QHostAddress& address, unsigned short port);

    /**
     * @brief login - this method try login of user.
     * @param userMail - the user email or unique id
     * @param rawPath - this is string of user path (not hash)
     * @return true if function finished successful
     */
    bool login(const QString& userMail, const QByteArray& rawPath);

    /**
     * @brief logout - logaut from user acount
     * @return true if function finished successful
     */
    bool logout();

    /**
     * @brief syncUserData - this method send request for update user data to server (save current data)
     * @return true if function finished successful
     */
    bool syncUserData();

    /**
     * @brief status
     * @return status of node
     */
    Q_INVOKABLE int status() const;

    /**
     * @brief lastMessage
     * @return last string massege
     */
    Q_INVOKABLE QString lastMessage() const;

protected:
    /**
     * @brief registerSocket this implementation connect new node to this object
     * @return true if finished seccussful
     */
    bool registerSocket(QAbstractSocket *socket, const QHostAddress* clientAddress) override;

    /**
     * @brief connectToHost this implementation update host information of client
     * @param ip
     * @param port
     * @param mode
     */
    void connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode = SslMode::NoSSL) override;
    void connectToHost(const QString &domain, unsigned short port, SslMode mode = SslMode::NoSSL) override;

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

    /**
     * @brief statusChanged - this signal emited when client object changed online status.
     * @param status
     */
    void statusChanged(int status);

    /**
     * @brief lastMessageChanged - this signal emited when client have a new message
     * @param lastMessage
     */
    void lastMessageChanged(QString lastMessage);
};

}

#endif // CLIENT_H
