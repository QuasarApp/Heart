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
 *
 * @note The client have next syntetic limitation:
 *  1. Maximum connection count = 1
 *  2. Connect possible only for one server overridden in the serverAddress method.
 *  3. addNode and removeNode methods unavalable in the client node.
 *
 *
 */
class HEARTSHARED_EXPORT SingleClient: public SingleBase
{
    Q_OBJECT
public:
    typedef std::function<void (const QSharedPointer<const PKG::AbstractData> &)> HandlerMethod;
    typedef QHash<unsigned int, HandlerMethod> HandlersCache;

    SingleClient();


    bool addNode(const HostAddress& address) = delete;
    bool addNode(const QString& address, unsigned short) = delete;
    bool removeNode(const HostAddress& address) = delete;

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
     *  If you want login uses your local user data just sets userId is empty. This method try login uses data from the getMember method.
     * @param rawPassword This is raw passwork of the userId. This method calc hash from the rawPassword and send getting value to server. For calculated hash uses the hashgenerator method. If you want to override own behavior for hashing or add an own salt then you need to override the hashgenerator method.
     * @return true if the user send authorization request successful.
     */
    bool login(const QString &userId = {}, const QString &rawPassword = {});

    /**
     * @brief login This method try to login client with new memberData by token.
     * @param memberData This is new member data, the client try login.
     * @return true if the user send authorization request successful.
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
     *  If you want to override own behavior for hashing or add an own salt then you need to override the hashgenerator method.
     * @return true if the user send registration request successful.

    */
    bool signup(const QString &userId, const QString &rawPassword);

    /**
     * @brief removeUser This method send to server the remove user and user data request.
     * @return true if the user send remove request successful.
     */
    bool removeUser();

    /**
     * @brief connectToServer This method try connect client or server.
     * @return true if the connect request send successful.
     */
    bool connectToServer();

    /**
     * @brief disconnectFromServer This method disconnect the client from server.
     */
    void disconnectFromServer();

    /**
     * @brief getLastError return last error code.
     * @return last error code.
     */
    ErrorCodes::Code getLastError() const;

    /**
     * @brief getLastErrorString This method return the string implementation of a last code error.
     * @return string value of the last error.
     */
    QString getLastErrorString() const;

    /**
     * @brief isConnected This method return true if the client is connected to server.
     * @return true if the server has "Connected" status.
     */
    bool isConnected() const;

    /**
     * @brief isLogined This method return true if the client is logged.
     * @return true if the server has "Logged" status.
     */
    bool isLogined() const;

    /**
     * @brief subscribe This method subscribe current logged user to the object with @a id.
     * @param id This is subscriber id of the object.
     * @return true if subscribe request sent successful.
     * \sa { syncSybscribeListWithServer, unsubscribe, isSubscribed, subscribesList }
     */
    bool subscribe(unsigned int id);

    /**
     * @brief unsubscribe This method unsubscribe current logged user to the object with @a id.
     * @param id This is unsubscribe id of the object.
     * @return true if unsubscribe request sent successful.
     * \sa { syncSybscribeListWithServer, subscribe, isSubscribed, subscribesList }
     */
    bool unsubscribe(unsigned int id);

    /**
     * @brief subscribesList This method returns local reference to local list with subsribes.
     * @warning This list updated when invoked subscribe and unsubscribe methods, so if your client have a unstable network connectin this value may be wrong.
     * @return lvalue to a subscrubes ids list.]
     *
     * @note This method is thread safe, so if you want check is subscribe only then use the SingleClient::isSubscribed method.
     * \sa { syncSybscribeListWithServer, subscribe, isSubscribed, unsubscribe }

     */
    QSet<unsigned int> subscribesList() const;

    /**
     * @brief isSubscribed This method check object with @a subscribeId to isSubscribed.
     * @warning This method use the local subscribed list, so it is may be works wrong if your client have a unstable network connection this value may be wrong.
     * @return true if the your client subscribed to subscribeId.
     * \sa { unsubscribe, subscribe, syncSybscribeListWithServer, subscribesList }
     */
    bool isSubscribed(unsigned int subscribeId) const;

    /**
     * @brief syncSybscribeListWithServer This method sends to server rest request to get subscribers list.
     * @param cb This is call back functions with a new subcribes list.
     * @return true if the reqest sendet successful.
     * \sa { unsubscribe, subscribe, isSubscribed }
     */
    bool syncSybscribeListWithServer(const std::function<void (const QSet<unsigned int>&)> &cb = {});

    /**
     * @brief restRequest This method send to server rest request and if the server send response invoke a handler method.
     * @param req This is request object. This maybe any objects from the PKH name space.
     * @param handler This is lambda function. This function invoked when client receive from server response of the sent request.
     * @return True if the request sent successfull.
     *
     * @b Example
     *  @code{cpp}
     *  auto cb = [](const QSharedPointer<PKG::AbstractData> &pkg) {
     *      ...
     *
     *  };
     *
     *      MyDataRequest request;
            request.setId(userId);
     *
     *  restRequest()
     *  @endcode
     */
    bool restRequest(PKG::AbstractData *req, const HandlerMethod& handler);


signals:
    /**
     * @brief statusChanged This signal emitted when the client change an own status.
     * @param status This is new status of the client.
     *  For more information about the statuses see the ClientStatus enum.
     */
    void statusChanged(QH::ClientStatus status);

    /**
     * @brief currentUserChanged This method invoked when user is logged and receive new token from server.
     * handle this signal if you want to save token in to database.
     */
    void currentUserChanged();

protected:
    /**
     * @brief setStatus This method sets a new status of the client. This method will be emitted the statusChanged signal.
     * @param status This is new status of the client.
     * @note The ClientStatus::Loginning and ClientStatus::Connecting statuses create a singleshot timer for check if the server response times out.
     */
    void setStatus(const ClientStatus &status);

    /**
     * @brief getMember This method return the UserMember object of logged user.
     * @return UserMember object.
     */
    const PKG::UserMember &getMember() const;

    /**
     * @brief setLastError This method sets new error code.
     * @param lastError This is new error code.
     */
    void setLastError(const ErrorCodes::Code &lastError);

    /**
     * @brief serverAddress This method return the pair {ip/domain,port} of server.
     *  Override this method for change server address.
     *  Default implementation return the localhost address with the 3090 port.
     * @return host of the server.
     */
    virtual QPair<QString, unsigned short> serverAddress() const;

    /**
     * @brief signPackageWithToken This method insert token into sending package. The package @a pkg should be inherited  of the IToken interface. IF @a pkg do not have a Itoken parent then this method ignore this validation and return true.
     * @param pkg This is pointer to the package object.
     * @note The @a pkg object will be changed after invoke this method.
     * @return true if the package get a token successful or package not support token validation else false.
     */
    bool signPackageWithToken(PKG::AbstractData *pkg) const;

    /**
     * @brief resetUser This method remove user member data and sets new status for this node (client).
     *  If user alredy loggined then status will be changed to connected.
     */
    void resetUser();

    void nodeConfirmend(AbstractNodeInfo *node) override;
    void nodeConnected(AbstractNodeInfo *node) override;
    void nodeDisconnected(AbstractNodeInfo *node) override;

    unsigned int sendData(PKG::AbstractData *resp,
                          const HostAddress &address,
                          const Header *req = nullptr) override;

    unsigned int sendData(PKG::AbstractData *resp,
                          const QVariant &nodeId,
                          const Header *req = nullptr) override;

    /**
     * @brief realServerAddress This method return the real server address that connected with this node.
     * @return The real server address that connected with this node.
     */
    const HostAddress& realServerAddress() const;

protected slots:
    void nodeErrorOccured(AbstractNodeInfo *nodeInfo,
                          QAbstractSocket::SocketError errorCode,
                          QString errorString) override;

private slots:
    /**
     * @brief handleError This handle method invoked when the client received the BadRequest from server.
     *  Override this method for add actions for this event.
     * @param code This is number of the error code.
     * @param error This is test message from a server.
     */
    void handleError(unsigned char code, QString error);
    // AbstractNode interface
private:

    /**
     * @brief setMember This method sets member data. Use this method if you restore your account data from the local storage.
     * @param member This is new network member data.
     */
    void setMember(const PKG::UserMember &member);

    bool p_login(const QString &userId, const QByteArray &hashPassword = {});
    bool p_signup(const QString &userId, const QByteArray &hashPassword);

    void handleRestRequest(const QSharedPointer<PKG::AbstractData> &pkg, const Header &pkgHeader);
    void setRealServerAddress(const HostAddress &realServerAddress);

    void addToSubscribesList(unsigned int id);
    void removeFromSubscribesList(unsigned int id);
    void setSubscribersList(QSet<unsigned int> ids);

    mutable QMutex _subscribesMutex;
    QSet<unsigned int> _subscribes;

    ClientStatus _status = ClientStatus::Dissconnected;
    QMutex _handlerMemberMutex;

    PKG::UserMember _member;
    ErrorCodes::Code _lastError = ErrorCodes::NoError;

    QMutex _handlerCacheMutex;
    HandlersCache _handlersCache;

    HostAddress _realServerAddress;


};

}
Q_DECLARE_METATYPE(QH::ClientStatus)

#endif // SINGLECLIENT_H
