/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SINGLECLIENT_H
#define SINGLECLIENT_H

#include <usermember.h>
#include "dberrorcodes.h"
#include "singlebase.h"

namespace QH {

/**
 * @brief The ClientStatus enum contains network statuses of the client.
 */
enum class ClientStatus: unsigned char {
    /// The Client is offline and do not tried connecting to server. (offline  mode)
    Dissconnected,
    /// The Client is offline but the connectToServer method has been invoked.
    /// This is mode of wait to connecting .
    Connecting,
    /// The Client successfully connected to server.
    /// This is Online mode. In This mode client is ready to work with server but in read only mode.
    Connected,
    /// The Client successfully connected to server and the login method has been invoked.
    /// This is mode of wait of logging.
    Loginning,
    /// The client successfully Loggined and ready to work with server.
    Logined
};


/**
 * @brief The SingleClient class This class provide login and singup user functionality for the SingleServer class.
 *
 * @note All pacakges that will be processed in this class should be sopport the token validation.
 * For get more information about token validation see the IToken class.
 */
class SingleClient: public SingleBase
{
    Q_OBJECT
public:
    SingleClient();

    QH::ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                  const Header& pkgHeader,
                                  const AbstractNodeInfo* sender) override;

    /**
     * @brief getStatus This method return current status of the client.
     *  For get more information see the ClientStatus enum.
     * @return status of the client.
     */
    ClientStatus getStatus() const;

    /**
     * @brief login This method try to loggin client.
     * @param userId This is userID. In The base implementation it is String value.
     *  The userId must be unique for all users.
     *  If You want you can use the email address of the user as a user id.
     *  If you want login uses your local user data jusst sets userId is empty. This method try login uses data from the getMember method.
     * @param rawPassword This is raw passwork of the userId. This method calc hash from the rawPassword and send getting value to server. For calculated hash uses the hashgenerator method. If you want to oveeride own behaviof for hashing or add an own salt then you need to override the hashgenerator method.
     * @return true if the user send autorisation request successful.
     */
    bool login(const QString &userId = {}, const QString &rawPassword = {});

    /**
     * @brief login This method try to loggin client with new memberData by token.
     * @param memberData This is new member data, the client try login.
     * @return true if the user send autorisation request successful.
     */
    bool login(const PKG::UserMember& memberData);

    /**
     * @brief logout This method send to server request for logout.
     * @return true if the user send logout request successful.
     */
    bool logout();

    /**
     * @brief signup This method try to register new user.
     * @param userId This is userID. In The base implementation it is String value.
     *  The userId must be unique for all users.
     *  If You want you can use the email address of the user as a user id.
     * @param rawPassword This is raw passwork of the userId.
     *  This method calc hash from the rawPassword and send getting value to server.
     *  For calculated hash uses the hashgenerator method.
     *  If you want to oveeride own behaviof for hashing or add an own salt then you need to override the hashgenerator method.
     * @return true if the user send registration request successful.

    */
    bool signup(const QString &userId, const QString &rawPassword);

    /**
     * @brief removeUser This method send to server the remove user and user datat request.
     * @return true if the user send remove request successful.
     */
    bool removeUser();

    /**
     * @brief connectToServer This method try connect client ot server.
     * @return true if the connect request sendet successful.
     */
    bool connectToServer();

    /**
     * @brief disconnectFromServer This method disconnect the clinet from server.
     */
    void disconnectFromServer();

    /**
     * @brief getLastError return last error code.
     * @return last error code.
     */
    ErrorCodes::Code getLastError() const;

    /**
     * @brief getLastErrorString This method return the string implementation of a last code error.
     * @return string calue of the last error.
     */
    QString getLastErrorString() const;

    /**
     * @brief isConnected This method retrun true if the client is connected to server.
     * @return true if the server has "Connected" status
     */
    bool isConnected() const;

    /**
     * @brief isLogined This method return true if the client is logined.
     * @return true if the server has "Logined" status
     */
    bool isLogined() const;

signals:
    /**
     * @brief statusChanged This sigmnal emited when the client change an own status.
     * @param status This is new status of the client.
     *  For more information about the statuses see the ClientStatus enum.
     */
    void statusChanged(QH::ClientStatus status);

protected:
    /**
     * @brief setStatus This method sets a new status of the client. This method will be emited the statusChanged signal.
     * @param status This is new status of the client.
     * @note The ClientStatus::Loginning and ClientStatus::Connecting statuses create a singleshot timer for check if the server response times out.
     */
    void setStatus(const ClientStatus &status);

    /**
     * @brief getMember This method return the UserMember object of loffined user.
     * @return UserMember object.
     */
    const PKG::UserMember &getMember() const;

    /**
     * @brief setLastError This method sets new error code.
     * @param lastError This is new error code.
     */
    void setLastError(const ErrorCodes::Code &lastError);

    /**
     * @brief serverAddress This method return the addres of server.
     *  Override this method for change server address.
     *  Default implementation return the localhost address with the 3090 port.
     * @return host of the server.
     */
    virtual HostAddress serverAddress() const;

    void nodeConfirmend(AbstractNodeInfo *node) override;
    void nodeConnected(AbstractNodeInfo *node) override;
    void nodeDisconnected(AbstractNodeInfo *node) override;

    bool sendData(const PKG::AbstractData *resp,
                  const HostAddress &address,
                  const Header *req = nullptr) override;

private slots:
    /**
     * @brief handleError This handle method invoked when the client received the BadRequest from server.
     *  Ovveride this method for add actions for this event.
     * @param code This is number of the error code.
     * @param error This is test message from a server.
     */
    void handleError(unsigned char code, QString error);

private:

    /**
     * @brief setMember This method sets memeber data. Use this method if you restore your account data from the local storage.
     * @param member This is new network member data.
     */
    void setMember(const PKG::UserMember &member);

    bool p_login(const QString &userId, const QByteArray &hashPassword = {});
    bool p_signup(const QString &userId, const QByteArray &hashPassword);

    ClientStatus _status = ClientStatus::Dissconnected;
    PKG::UserMember _member;
    ErrorCodes::Code _lastError = ErrorCodes::NoError;


};

}
Q_DECLARE_METATYPE(QH::ClientStatus)

#endif // SINGLECLIENT_H
