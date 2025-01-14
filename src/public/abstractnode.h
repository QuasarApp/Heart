﻿/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H

#include "abstractnodeinfo.h"
#include "ping.h"

#ifdef USE_HEART_SSL
#include <QSslConfiguration>
#include <easyssl/icertificate.h>
#endif

#include <QAbstractSocket>
#include <QFutureWatcher>
#include <QMutex>
#include <QSharedPointer>
#include <QTcpServer>
#include <QThreadPool>
#include <QTimer>
#include <softdelete.h>
#include "abstractdata.h"
#include "workstate.h"
#include "package.h"
#include "heart_global.h"
#include <iparser.h>

namespace QH {

class DataSender;
class ReceiveData;
class SocketFactory;
class AsyncLauncher;
class TaskScheduler;
class AbstractTask;
class SslSocket;
class APIVersionParser;

namespace PKG {
class ErrorData;
}

/**
 * @brief The SslMode enum  This enum contatins options for set ssl mode of node (server).
 * For more information see AbstractNode::useSelfSignedSslConfiguration AbstractNode::useSystemSslConfiguration and AbstractNode::disableSSL methods.
 */
enum class SslMode {
    /// This is not secure connection without ssl encription. It is default value of new any node see AbstractNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr).
    NoSSL,
#ifdef USE_HEART_SSL

    /// This option try enable ssl connection from system configuration form fore information see Qt Documentation https://doc.qt.io/qt-5/qsslconfiguration.html
    InitFromSystem,
    /// This option force a current node geneerate self signed sertificat and work with it. For more information see a SslSrtData struct.
    InitSelfSigned
#endif
};

/**
 * @brief The AddNodeError enum contains error code that can be occured after invoke the AbstractNode::addNode mehod
 * @see AbstractNode::addNode
 * @see AbstractNode::addNodeFailed
 * @see AbstractNode::nodeAddedSucessful
 */
enum class AddNodeError {
    /// unknown error
    UnknownError,
    /// This error ocurred when DNS server not responce to node or node can't find the server ip address by host.
    HostNotFound,
    /// This error ocurred when you try add baned node or server is overrload.
    RegisterSocketFailed
};

#define CRITICAL_ERROOR  -50
#define LOGICK_ERROOR    -20
#define REQUEST_ERROR    -5
#define NOTSUPPORT_ERROR -1

#define BIG_DATA_HASH_ID 0xFFFF

class Abstract;

/**
 * @brief The AbstractNode class - Abstract implementation of node.
 *  this implementation have a methods for send and receive data messages,
 *  and work with crypto method for create a security connections betwin nodes.
 *  AbstractNode - is thread save class.
 *
 * @note For correctly working this class you should be register all you dataParser types using the AbstractNode::addApiParser method.
 *  @see AbstractData
 *  @see iParser
 *  @see AbstractNode::addApiParser
 */
class HEARTSHARED_EXPORT AbstractNode : public QTcpServer, public SoftDelete
{
    Q_OBJECT

public:

    /**
     * @brief The NodeType enum contains types of the node. By default node contains only 3 types.
      */
    enum NodeType: int {
        /// Node with this type is general clints nodes.
        Client = 0,

        /// Node with this ytpe is midle nodes that can works as a client and as servers.
        Node = 1,

        /// This is node can works only as a public server
        Server = 2
    };

    /**
     * @brief AbstractNode - Base constructor of node.
     * @param ptr - Pointrt to parent Qt object, the AbstractNode class is Q_OBJECT.
     * @note For correctly working this class you should be register all you data types using the AbstractNode::registerPackageType method.
     */
    AbstractNode(QObject * ptr = nullptr);
    ~AbstractNode() override;

    /**
     * @brief nodeType This method should be return type of the serve.
     * @return node type q qq
     */
    virtual NodeType nodeType() const = 0;

    /**
     * @brief run This method implement deployment a network node (server) on selected address.
     * @param addres This is network address of work node or server.
     *  If address is empty then server weel be listen all addreses of all interfaces else listen only selected address.
     * @param port This is port of deployment node (server)
     * @return Result of deployment node (sever). (True if deploy finished successful else false).
     */
    virtual bool run(const QString& addres, unsigned short port);

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param cmd this is command that need to parse.
     * @param sender this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(unsigned short cmd,
                                             AbstractNodeInfo *sender) const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param type this is parser type.
     * @param version this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(const QString& type,
                                             int version) const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param type this is parser type.
     * @param sender this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(const QString& type,
                                             AbstractNodeInfo *sender) const;

    /**
     * @brief stop - Stopped this node and close all network connections.
     */
    virtual void stop();

    /**
     * @brief getInfoPtr - This method return information class pointer about netwok connection.
     *  If Connection with id not found then return nullptr.
     * @param id - It is network address of requested node.
     * @return The pointer of information about node. if address not found return nullptr.
     */
    virtual AbstractNodeInfo* getInfoPtr(const HostAddress &id);

    /**
     * @brief getInfoPtr - This is some that getInfoPtr(const HostAddress &id) bod it is constant implementation.
     * @param id - It is network address of requested node.
     * @return The pointer of information about node. if address not found return nullptr.
     */
    virtual const AbstractNodeInfo* getInfoPtr(const HostAddress &id) const;

    /**
     * @brief ban - This method set for target connection a trust property to 0 and target connection will been aborted.
     * @param target - It is network address of target connection.
     */
    virtual void ban(const HostAddress& target);

    /**
     * @brief unBan - This method set for target connection a trust property to 100.
     * @param target - It is network address of target connection.
     */
    virtual void unBan(const HostAddress& target);

    /**
     * @brief sendData This method send data  object another to node
     * @param resp This is pointer to sendet object.
     * @param address This is target addres for sending.
     * @param req This is header of request.
     * @return hash of the sendet package. If function is failed then return 0.
     */
    virtual unsigned int sendData(const PKG::AbstractData *resp,  const HostAddress& address,
                                  const Header *req = nullptr);

    /**
     * @brief sendData This method send data  object another to node
     * @param resp This is pointer to sendet object.
     * @param address This is target addres for sending.
     * @param req This is header of request.
     * @return hash of the sendet package. If function is failed then return 0.
     */
    virtual unsigned int sendData(const PKG::AbstractData *resp, const AbstractNodeInfo *node,
                                  const Header *req = nullptr);

    /**
     * @brief addNode - Connect to node (server) with address.
     * @param address - This is Network address of node (server).
     * @return true if the node aaded successful
     * @see AbstractNode::nodeAddedSucessful
     * @see AbstractNode::addNodeFailed
     */
    bool addNode(const HostAddress &address);

    /**
     * @brief addNode This method add new peer connection to this node and
     *  execute the @a action when node status will be changed to the @a status.
     * @param address This is address of peer node.
     * @param action This is action that will be executed when status will changed to required status.
     * @param status This is required status.
     * @return true if node added successful.
     *
     * @note The Action do not executed when node alredy connected.
     * @note If you override the nodeConfirmend or the nodeConnected method then you must be invoke parent implementation else @a action do not executed.
     * @see AbstractNode::nodeAddedSucessful
     * @see AbstractNode::addNodeFailed
     */
    bool addNode(const HostAddress &address,
                 const std::function<void(QH::AbstractNodeInfo *)>& action,
                 NodeCoonectionStatus status = NodeCoonectionStatus::Connected);

    /**
     * @brief addNode This method add new peer connection to this node and
     *  execute the @a action when node status will be changed to the @a status.
     * @param domain - This is domain address of node (server).
     * @param port - This is target port of node (server).
     * @param action This is action that will be executed when status will changed to required status.
     *  You can ignore this argument for disable actions after connect.
     * @param status This is required status.
     *  You can ignore this argument. By default it is Connected state
     * @return true if node added successful.
     *
     *
     *  **Example of use:**
     * @code{cpp}
     *
     * @endcode
     *
     * @note The Action do not executed when node alredy connected.
     * @note If you override the nodeConfirmend or the nodeConnected method then you must be invoke parent implementation else @a action do not executed.
     * @see AbstractNode::nodeAddedSucessful
     * @see AbstractNode::addNodeFailed
     */
    bool addNode(const QString &domain, unsigned short port,
                 const std::function<void(QH::AbstractNodeInfo *)>& action = nullptr,
                 NodeCoonectionStatus status = NodeCoonectionStatus::Connected);

    /**
     * @brief removeNode - Remove node and disconnected forom node (server).
     * @param nodeAdderess - This is network adddress of removed node (server).
     * @return true if the node removed successful. If the nde with @a nodeAdderess is not exits return false.
     */
    bool removeNode(const HostAddress& nodeAdderess);

    /**
     * @brief removeNode - Remove node and disconnected forom node (server).
     * @param node - This is removed node (server).
     * @return true if the node removed successful. If the nde with @a nodeAdderess is not exits return false.
     */
    bool removeNode(AbstractNodeInfo* node);

    /**
     * @brief address - Thim method return own network address of current node (server).
     * @return The current network adderss.
     */
    HostAddress address() const;

#ifdef USE_HEART_SSL

    /**
     * @brief getSslConfig - This method return ssl configuration of current node (server).
     * @return current ssl configuration on this node (server).
     */
    QSslConfiguration getSslConfig() const;
#endif
    /**
     * @brief getMode - This method return SSL mode of corrent node (server).
     * @return current mode for more information see SslMode.
     */
    SslMode getMode() const;

    /**
     * @brief getWorkState - This method collect general information about this server.
     *  For more information about returned data see getWorkState
     * @return state value for more information see WorkState class.
     */
    virtual WorkState getWorkState() const;

    /**
     * @brief connectionsCount - Return count fo connections (connections with status connected)
     * @return Count valid connections.
     */
    int connectionsCount() const;

    /**
     * @brief connectionsCount - Return count of nodes with status confirmend.
     * @return return confirmend connections of this node (server).
     */
    int confirmendCount() const;

    /**
     * @brief ping This method send ping package to address for testing connection.
     * @param address This is address of target node (server).
     * @return true if ping sendet successful.
     */
    bool ping( const HostAddress& address);

    /**
     * @brief mainThreadID This method return the pointer to main thread
     * @return pointer to main thread
     */
    static QThread *mainThreadID();

    /**
     * @brief sheduleTask This method shedule execute task on this node.
     * @param task This is task that will be sheduled.
     * @return true if task added successfull else flase
     * @see AbstractNode::removeTask
     * @see AbstractNode::sheduledTaskCount

     */
    bool sheduleTask(const QSharedPointer<AbstractTask>& task);

    /**
     * @brief removeTask This method remove task from sheduler.
     * @param taskId This is task id that will be removed.
     * @see AbstractNode::sheduleTask
     * @see AbstractNode::sheduledTaskCount
     */
    void removeTask(int taskId);

    /**
     * @brief sheduledTaskCount This method return count of sheduled tasks.
     * @return count of sheduled tasks.
     * @see AbstractNode::sheduleTask
     * @see AbstractNode::removeTask
     */
    int sheduledTaskCount() const;

    /**
     * @brief badRequest This method is send data about error of request.
     * @param address This is addrees of receiver.
     * @param req This is header of incomming request.
     * @param err This is message and code of error. For more information see the ErrorData struct.
     * @param diff This is difference of current trust (currenTrus += diff).
     * By default diff equals REQUEST_ERROR
     */
    virtual void badRequest(const HostAddress &address, const Header &req,
                            const PKG::ErrorData& err, qint8 diff = REQUEST_ERROR);

#ifdef USE_HEART_SSL

    /**
     * @brief ignoreSslErrors This method return list of ignored ssl errors
     * @return list of ignored ssl errors.
     * @see handleSslErrorOcurred
     */
    const QList<QSslError> &ignoreSslErrors() const;
#endif

    /**
     * @brief addApiParser This method add new Api parser for this node.
     * @param parserObject This is bew api parser.
     * @return added parser.
     */
    bool addApiParser(const QSharedPointer<iParser>& parser);

    /**
     * @brief fSendBadRequestErrors This property enable or disable sending feedback to connected node when them sent wrong packages.
     * @return true if the sending feedbacks is enabled and false if disabled.
     */
    bool fSendBadRequestErrors() const;

    /**
     * @brief setSendBadRequestErrors This method enable or disable the fSendBadRequestErrors property.
     * @param value This is new value of the sendBadRequestErrors property.
     */
    void setSendBadRequestErrors(bool value);

    /**
     * @brief fCloseConnectionAfterBadRequest This propery enable or disable droping connection after badRequests. By default it is true.
     * @return true if the clossing connection is enabled and false if disabled.
     */
    bool fCloseConnectionAfterBadRequest() const;

    /**
     * @brief setSendBadRequestErrors This method enable or disable the fcloseConnectionAfterBadRequest property.
     * @param value This is new value of the fcloseConnectionAfterBadRequest property.
     */
    void setCloseConnectionAfterBadRequest(bool value);

signals:
    /**
     * @brief requestError This signal emited when client or node received from remoute server or node the BadRequest package.
     * @param code This is code of error.
     * @param msg - received text of remoute node (server).
     */
    void requestError(unsigned char code, QString msg);

    /**
     * @brief sigNoLongerSupport is some as a APIVersionParser::sigNoLongerSupport. This signal just retronslate this signal as direct connection.
     * @param ApiKey see the APIVersionParser::sigNoLongerSupport description
     * @param version see the APIVersionParser::sigNoLongerSupport description
     * @see APIVersionParser::sigNoLongerSupport
     */
    void sigNoLongerSupport(const QString& ApiKey, unsigned short version);


protected:

#ifdef USE_HEART_SSL

    /**
     * @brief setIgnoreSslErrors This list sets new ignored errors.
     * @param newIgnoreSslErrors This is new list of ignored errors.
     * @see handleSslErrorOcurred
     */
    void setIgnoreSslErrors(const QList<QSslError> &newIgnoreSslErrors);

    /**
     * @brief selfSignedSslConfiguration This method create a new ssl configuration with selfsigned certificates.
     * @param data This is data for generate selfsigned certification for more information see SslSrtData structure.
     * @return The new selfsigned ssl configuration.
     */
    virtual QSslConfiguration selfSignedSslConfiguration( const EasySSL::SslSrtData& data = {});
#endif

    /**
     * @brief createNodeInfo This method create a nodeInfo object.
     *  override this method for create your own nodeInfo objects. for more in
     * @param socket This is socket of network address.
     * @param clientAddress This parameter need to set when the socket du not contains a address or invalid.
     * @return return pointer to info object.
     */
    virtual AbstractNodeInfo* createNodeInfo(QAbstractSocket *socket,
                                             const HostAddress *clientAddress = nullptr) const;

    /**
     * @brief registerSocket This method registration new socket object.
     * @param socket This is incomming socket pointer.
     * @param address This is host address of socket. By default is nullptr.
     *  Set this value for nodes created on this host.
     * @return return true if the scoeket has been registered successful.
     */
    virtual bool registerSocket(QAbstractSocket *socket, const HostAddress* address = nullptr);

    /**
     * @brief sendPackage This method prepare and send to target address a package.
     * @param pkg This is sendet pakcage to target node.
     * @param target This is target node.
     * @return return true if The package is sendet succesfull.
     *
     * @note All packages sendets on the sender threaed. But thread of the node is wait for sends result.
     *  This is done that allthe data that is sent when node are dissconected come without fail.
     */
    virtual bool sendPackage(const Package &pkg, QAbstractSocket *target) const;

    /**
     * @brief getWorkStateString This method generate string about work state of server.
     * @return string of work state.
     */
    virtual QString getWorkStateString() const;

    /**
     * @brief connectionState This method return string value about the cocction state.
     * @return string with count users state.
     */
    virtual QString connectionState() const;

    /**
     * @brief banedList This method retrun list of banned clients of nodes.
     * @return list of baned nodes.
     */
    QList<HostAddress> banedList() const;

    // TO-DO Need to add new method fo collect banned addresses for exmaple use the mask.
    // See Task https://github.com/QuasarApp/Heart/issues/13
    /**
     * @brief isBanned This method checks if the node is banned.
     * @param socket This is node info object for validation.
     * @return true if node is banned.
     */
    virtual bool isBanned(const AbstractNodeInfo *socket) const;

    /**
     * @brief incomingConnection This is ovverided method of QTCPServer.
     * @param handle This is socket handle.
     */
    void incomingConnection(qintptr handle) override final;

    /**
     * @brief changeTrust This method change trust of connected node.
     * @param id This is id of select node.
     * @param diff This is difference of current trust (currenTrus += diff).
     * @return true if node trust is changed successful.
     */
    virtual bool changeTrust(const HostAddress& id, int diff);


#ifdef USE_HEART_SSL

    /**
     * @brief useSelfSignedSslConfiguration This method reconfigure current node to use selfSigned certificate.
     * @note Befor invoke this method stop this node (server) see AbstractNode::stop.
     *  if mode will be working then this method return false.
     *  The self signed certificate is temp value, this is will be changed after reboot node (server)
     * @param crtData - This is data for generation a new self signed certification.
     * @return result of change node ssl configuration.
     */
    bool useSelfSignedSslConfiguration(const EasySSL::SslSrtData& crtData);

    /**
     * @brief useSystemSslConfiguration This method reconfigure current node to use sslConfig.
     * @note Befor invoke this method stop this node (server) see AbstractNode::stop.
     *  if mode will be working then this method return false.
     *  @param config This is system ssl configuration. by default used QSslConfiguration::defaultConfiguration() method.
     * @return result of change node ssl configuration.
     */
    bool useSystemSslConfiguration(QSslConfiguration config = QSslConfiguration::defaultConfiguration());

    /**
     * @brief disableSSL This method disable ssl mode for this node.
     * @note Befor invoke this method stop this node (server) see AbstractNode::stop.
     *  if mode will be working then this method return false.
     * @return true if changes is completed.
     */
    bool disableSSL();
#endif

    /**
     * @brief connections - Return hash map of all connections of this node.
     * @return return map of connections.
     */
    QHash<HostAddress, AbstractNodeInfo *> connections() const;

    /**
     * @brief nodeConfirmend This method invocked when the node status changed to "confirmend"
     *  default implementatio do nothing.
     * @param node This is address of changed node.
     */
    virtual void nodeConfirmend(AbstractNodeInfo *node);

    /**
     * @brief nodeConnected This method invocked when the node status changed to "connected"
     *  default implementatio do nothing.
     * @param node This is address of changed node.
     */
    virtual void nodeConnected(AbstractNodeInfo *node);

    /**
     * @brief nodeConnected This method invocked when the node status changed to "disconnected"
     *  default implementatio do nothing.
     * @param node This is address of changed node.
     */
    virtual void nodeDisconnected(AbstractNodeInfo *node);

    void prepareForDelete() override;

    /**
     * @brief prepareData This is private method for preparing package from the byteArray.
     * @param pkg This is a raw package value.
     * @param sender This is sender of the @a pkg.
     * @return pointer into prepared data.
     * @warning The return value do not clear automatically.
     */
    QSharedPointer<PKG::AbstractData> prepareData(const Package& pkg, AbstractNodeInfo *sender) const;

    /**
     * @brief connectionsList This method return list of all node connections
     * @return list of node connections.
     * @warning do not use this method for validation is connected.
     */
    QList<HostAddress> connectionsList() const;

    /**
     * @brief activeConnectionsList This method return list of actived nodes connections
     * @return list of actived nodes connections.
     * @warning do not use this method for validation is connected.
     */
    QList<HostAddress> activeConnectionsList() const;

    /**
     * @brief addNodeFailed This method will be invoked when trying to add new node are failed.
     *  So override this method for handle this event.
     * @param error This is error code that occured after invoke of the AbstractNode::addNode method
     * @see AbstractNode::addNode
     * @see AbstractNode::nodeAddedSucessful
     */
    virtual void addNodeFailed(AddNodeError error);

    /**
     * @brief nodeAddedSucessful This method will be invoked when new node added successful.
     * @param node This is pointer to added node.
     * @note do not try remove the @a node pointer. This pointer will be removed automaticaly.
     * @see AbstractNode::addNode
     * @see AbstractNode::addNodeFailed
     */
    virtual void nodeAddedSucessful(AbstractNodeInfo* node);

    /**
     * @brief addApiParserNative This is template metod that add sipport of new apiparser @a ApiType
     * @tparam ApiType This is type of new apiParser that will be added to the main parser.
     * @tparam Args This is argumets that will forward to the Parser constructor.
     * @return shared pointer to the @a ApiType
     */
    template<class ApiType, class ... Args >
    QSharedPointer<ApiType> addApiParserNative(Args&&... arg) {
        return addApiParserImpl(QSharedPointer<ApiType>::create(this, std::forward<Args>(arg)...)).template staticCast<ApiType>();
    }

    /**
     * @brief addApiParser This is template metod that add sipport of new apiparser @a ApiType
     * @tparam ApiType This is type of new apiParser that will be added to the main parser.
     * @tparam Args This is argumets that will forward to the Parser constructor.
     * @return shared pointer to the iParser
     */
    template<class ApiType, class ... Args >
    const QSharedPointer<iParser> & addApiParser(Args&&... arg) {
        return addApiParserImpl(QSharedPointer<ApiType>::create(this, std::forward<Args>(arg)...));
    }

protected slots:
    /**
     * @brief receivePing This method invoked when node receive new ping object.
     * @param ping This is ping object.
     */
    virtual void receivePing(const QSharedPointer<QH::PKG::Ping>& ping);;

    /**
     * @brief nodeErrorOccured This slot invoked when error ocured in the @a nodeInfo.
     * @param nodeInfo This is pinter to modeInfoObject.
     * @param errorCode This is error code.
     * @param errorString This is string value of the error.
     * @note default implementation do nothing. Override this method if you want to handle nodes network errors.
     */
    virtual void nodeErrorOccured(QH::AbstractNodeInfo *nodeInfo,
                                  QAbstractSocket::SocketError errorCode,
                                  QString errorString);

#ifdef USE_HEART_SSL

    /**
     * @brief handleSslErrorOcurred This method invoked when a ssl socket receive an error mesage.
     *  Default implementation just pront error messages
     *  Overrid this method for handle ssl errors on this node or server.
     * @param error This is error that occured..
     */
    virtual void handleSslErrorOcurred(QH::SslSocket *scket, const QSslError& error);
#endif

private slots:

    void avelableBytes(QH::AbstractNodeInfo* sender);

    /**
     * @brief handleNodeStatusChanged This method invoked when status of peer node chganged.
     * @param node This is address of changed node.
     * @param status This is new status of node.
     *
     */
    void handleNodeStatusChanged(QH::AbstractNodeInfo* node, QH::NodeCoonectionStatus status);

    /**
     * @brief handleWorkerStoped
     */
    void handleWorkerStoped();

    /**
     * @brief handleForceRemoveNode - force remove connection.
     * @param node
     */
    void handleForceRemoveNode(QH::HostAddress node);

    /**
     * @brief handleBeginWork This method run task on new thread.
     * @param work This is new work task
     */
    void handleBeginWork(QSharedPointer<QH::AbstractTask> work);

#ifdef USE_HEART_SSL

    /**
     * @brief handleSslErrorOcurredPrivate This method invoked when a ssl socket receive an error mesage.
     *  Default implementation just pront error messages
     * @param errors This is errors list.
     */
    void handleSslErrorOcurredPrivate(QH::SslSocket *sender, const QList<QSslError> & errors);

    /**
     * @brief handleEncrypted invoke when a ssl socket is encripted!
     */
    void handleEncrypted(QH::AbstractNodeInfo *node);

#endif

private:

    /**
     * @brief addApiParserImpl This method add new Api parser for this node.
     * @param parserObject This is bew api parser.
     * @return added parser.
     */
    const QSharedPointer<iParser> & addApiParserImpl(const QSharedPointer<QH::iParser>& parserObject);

    // iParser interface
    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              AbstractNodeInfo *sender);

    /**
     * @brief genPackage This method generate a package from the command and version
     * @param cmd this is command of the package.
     * @param sender This is sender that sent this requests.
     * @return empty package object.
     */
    QSharedPointer<PKG::AbstractData>
    genPackage(unsigned short cmd,
               AbstractNodeInfo *sender) const;

    /**
      @note just disaable listen method in the node objects.
     */
    bool listen(const HostAddress& address = HostAddress::Any);

    /**
     * @brief newWork - this method it is wraper of the parsePackage method.
     *  the newWork invoke a parsePackage in the new thread.
     * @param pkg
     * @param sender
     */
    void newWork(const Package &pkg, AbstractNodeInfo *sender, const HostAddress &id);

    /**
     * @brief checkConfirmendOfNode - this method remove old not confirmed node.
     * @param node - Node address.
     */
    void checkConfirmendOfNode(AbstractNodeInfo *node);

    /**
     * @brief initThreadId This method save curent thread as a main thread.
     */
    void initThreadId() const;

    /**
     * @brief initThreadPool This method initialize thread pool of the workers
     */
    void initThreadPool();

    /**
     * @brief deinitThreadPool This method remove all workers threads
     */
    void deinitThreadPool();


    SslMode _mode = SslMode::NoSSL;
#ifdef USE_HEART_SSL
    bool configureSslSocket(AbstractNodeInfo *node, bool fServer);
    QSslConfiguration _ssl;
    QList<QSslError> _ignoreSslErrors;
#endif
    QHash<HostAddress, AbstractNodeInfo*> _connections;
    QHash<HostAddress, ReceiveData*> _receiveData;

    DataSender * _dataSender = nullptr;
    AsyncLauncher * _socketWorker = nullptr;
    QThread *_senderThread = nullptr;
    TaskScheduler *_tasksheduller = nullptr;
    APIVersionParser *_apiVersionParser = nullptr;

    QHash<NodeCoonectionStatus,
          QHash<HostAddress,
                std::function<void (QH::AbstractNodeInfo *)>>> _connectActions;

    QSet<QFutureWatcher <bool>*> _workers;
    bool _sendBadRequestErrors = true;
    bool _closeConnectionAfterBadRequest = false;

    mutable QMutex _connectionsMutex;
    mutable QMutex _confirmNodeMutex;
    mutable QMutex _threadPoolMutex;
    mutable QMutex _workersMutex;

    QThreadPool *_threadPool = nullptr;

    friend class WebSocketController;
    friend class SocketFactory;
    friend class BigDataParser;
    friend class BigDataParserOld;
    friend class AbstractNodeParser;
    friend class AbstractNodeParserOld;


};

}
#endif // ABSTRACTNODE_H
