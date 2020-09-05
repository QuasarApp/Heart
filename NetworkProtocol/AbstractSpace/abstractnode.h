/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H

#include "abstractnodeinfo.h"
#include "networkprotocol.h"
#include <openssl/evp.h>

#include <QAbstractSocket>
#include <QFutureWatcher>
#include <QMutex>
#include <QSslConfiguration>
#include <QTcpServer>
#include <QTimer>
#include "abstractdata.h"
#include "workstate.h"

#include "cryptopairkeys.h"
#include "icrypto.h"
#include "networkprotocol_global.h"
#include "receivedata.h"

class QSslCertificate;
class QSslKey;
class QSslConfiguration;

namespace NP {

class DataSender;

/**
 * @brief The ParserResult enum
 * Error - parser detect a errorob package
 * NotProcessed - the parser does not know what to do with the package or has not finished processing it.
 * Processed - the parser finished processing correctly
 */
enum class ParserResult {
    /// parser detect a errorob package
    Error = 0,
    /// the parser does not know what to do with the package or has not finished processing it.
    NotProcessed = 1,
    /// the parser finished processing correctly
    Processed = 2
};

enum class SslMode {
    NoSSL,
    InitFromSystem,
    InitSelfSigned
};

/**
 * @brief The SslSrtData struct
 */
struct SslSrtData {
    QString country = "BY";
    QString organization = "QuasarApp";
    QString commonName = "Dev";
    long long endTime = 31536000L; //1 year
};


#define CRITICAL_ERROOR -50
#define LOGICK_ERROOR   -20
#define REQUEST_ERROR   -5

class Abstract;

/**
 * @brief The AbstractNode class - Abstract implementation of node.
 *  this implementation have a methods for send and receive data messages,
 *  and work with crypto method for crease a security connections betwin nodes.
 *  AbstractNode - is thread save class
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractNode : public QTcpServer
{
    Q_OBJECT

public:

    /**
     * @brief AbstractNode
     * @param ssl
     * @param ptr
     */
    AbstractNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);    
    ~AbstractNode() override;

    /**
     * @brief run
     * @param addres - If address is empty then serve weel be listen all addreses of all interfaces
     * @param port
     * @return true if all good
     */
    virtual bool run(const QString& addres, unsigned short port);

    /**
     * @brief stop stop this node and close all connections.
     */
    virtual void stop();

    /**
     * @brief getInfo
     * @param id of selected node
     * @return pointer to information about node. if address not found return nullpt
     */
    virtual AbstractNodeInfo* getInfoPtr(const HostAddress &id);

    /**
     * @brief getInfoPtr
     * @param id peer adders
     * @return pointer to information about node. if address not found return nullpt
     */
    virtual const AbstractNodeInfo* getInfoPtr(const HostAddress &id) const;

    /**
     * @brief ban
     * @param target id of ban node
     */
    virtual void ban(const HostAddress& target);

    /**
     * @brief unBan
     * @param target id of unban node
     */
    virtual void unBan(const HostAddress& target);

    /**
     * @brief connectToHost - connect to host node
     * @param address -  address of node
     * @param mode - mode see SslMode
     */
    virtual bool connectToHost(const HostAddress &address, SslMode mode = SslMode::NoSSL);

    /**
     * @brief connectToHost - connect to host node. this method find ip address of domain befor connecting
     * @param domain: address of node
     * @param port - port of node
     * @param mode - mode see SslMode
     */
    virtual bool connectToHost(const QString &domain, unsigned short port, SslMode mode = SslMode::NoSSL);

    /**
     * @brief addNode - add new node for connect
     * @param nodeAdderess - the network addres of a new node.
     */
    void addNode(const HostAddress& nodeAdderess);

    /**
     * @brief removeNode - remove node
     * @param nodeAdderess - the adddress of removed node.
     */
    void removeNode(const HostAddress& nodeAdderess);

    /**
     * @brief address - address of this node
     * @return return current adders
     */
    HostAddress address() const;

    /**
     * @brief getSslConfig - configuration of this node.
     * @return current ssl configuration on this nod
     */
    QSslConfiguration getSslConfig() const;

    /**
     * @brief getMode
     * @return
     */
    SslMode getMode() const;

    /**
     * @brief getWorkState
     * @return
     */
    virtual WorkState getWorkState() const;

    /**
     * @brief pareseResultToString
     * @return string of pareseresult
     */
    QString pareseResultToString(const ParserResult& parseResult) const;

    /**
     * @brief connectionsCount - return count fo connections (nodes with status connected)
     * @return
     */
    int connectionsCount() const;

    /**
     * @brief connectionsCount - return count of nodes with status confirmend
     * @return
     */
    int confirmendCount() const;

    /**
     * @brief ping - ping address for testing
     * @param address - address of other node
     * @return true if ping sendet
     */
    bool ping( const HostAddress& address);

signals:
    void requestError(QString msg);

protected:

    /**
     * @brief generateRSAforSSL
     * @param pkey -
     * @return
     */
    virtual bool generateRSAforSSL(EVP_PKEY* pkey) const;

    /**
     * @brief generateSslData - generate new ssl data
     * @param data - sign data
     * @param r_srt - result srt
     * @param r_key - result private key
     * @return true if all good
     */
    virtual bool generateSslDataPrivate(const SslSrtData& data, QSslCertificate& r_srt, QSslKey& r_key);

    /**
     * @brief selfSignedSslConfiguration
     * @return generate new keys and use it
     */
    virtual QSslConfiguration selfSignedSslConfiguration();

    /**
     * @brief createNodeInfo
     * @return nodeinfo for new connection
     * override this metho for set your own nodeInfo objects;
     */
    virtual AbstractNodeInfo* createNodeInfo(QAbstractSocket *socket,
                                             const HostAddress *clientAddress = nullptr) const;

    /**
     * @brief registerSocket - this method registration new socket
     * @param socket - socket pointer
     * @param incoming - host address of socket. by default is nullptr.
     *  Set this value for nodes created on this host
     * @return return true if the scoeket has been registered successful
     */
    virtual bool registerSocket(QAbstractSocket *socket, const HostAddress* address = nullptr);

    /**
     * @brief parsePackage
     * @param pkg
     * @param sender
     * @return item of ParserResult ()
     */
    virtual ParserResult parsePackage(const Package &pkg, const AbstractNodeInfo* sender);

    /**
     * @brief sendPackage
     * @param pkg
     * @param target
     * @return
     */
    virtual bool sendPackage(const Package &pkg, QAbstractSocket *target) const;

    /**
     * @brief sendData send data package to address and prepare object to sending.
     * @param resp - pointer to sendet object
     * @param address - target addres for sending
     * @param req - header of request
     * @return true if data sendet succesful.
     */
    virtual bool sendData(AbstractData *resp,  const HostAddress& addere,
                          const Header *req = nullptr);

    /**
     * @brief sendData this is some as a sendData(AbstractData *resp ...) exept this method not prepare object for sending.
     * @param resp - pointer to sendet object
     * @param address - target addres for sending
     * @param req - header of request
     * @return true if data sendet succesful.
     */
    virtual bool sendData(const AbstractData *resp,  const HostAddress& addere,
                          const Header *req = nullptr);

    /**
     * @brief badRequestu
     * @param address
     * @param req
     * @param msg - message of error
     */
    virtual void badRequest(const HostAddress &address, const Header &req,
                            const QString msg = "");

    /**
     * @brief getWorkStateString
     * @return string of work state
     */
    virtual QString getWorkStateString() const;

    /**
     * @brief connectionState
     * @return string with count users state
     */
    virtual QString connectionState() const;

    /**
     * @brief banedList
     * @return list of baned nodes
     */
    QList<HostAddress> banedList() const;

    /**
     * @brief isBanned
     * @param socket
     * @return
     */
    bool isBanned(QAbstractSocket* socket) const;

    /**
     * @brief incomingConnection
     * @param handle
     */
    void incomingConnection(qintptr handle) override;

    /**
     * @brief changeTrust change trust of connected node
     * @param id - id of select node
     * @param diff
     * @return true if all good
     */
    virtual bool changeTrust(const HostAddress& id, int diff);

    /**
    * @brief incomingConnection for ssl sockets
    * @param handle - handle of socket
    */
    virtual void incomingSsl(qintptr handle);

    /**
    * @brief incomingConnection for tcp sockets
    * @param handle -  handle of socket
    */
    virtual void incomingTcp(qintptr handle);


    /**
     * @brief setMode - invoke this method befor run method
     * @param mode
     */
    bool setMode(const SslMode &mode);

    /**
     * @brief incomingData - this signal invoked when node get command or ansver
     * @param pkg - get package (in this implementation it is only the Ping command)
     * @param sender - sender of the package
     * @note override this method for get a signals.
     */
    virtual void incomingData(AbstractData* pkg,
                      const HostAddress&  sender);

    /**
     * @brief connections - return hash map of all connections of this node.
     * @return
     */
    QHash<HostAddress, AbstractNodeInfo*> connections() const;

    /**
     * @brief connectionRegistered Override this method for get registered incoming connections.
     * @param info - connection information.
     */
    virtual void connectionRegistered(const AbstractNodeInfo *info);

    /**
     * @brief nodeStatusChanged - This method invoked when status of node chganged.
     *  Base implementation do nothing. Override this method for add own functionality.
     * @param node - address of changed node.
     * @param status - new status of node.
     *
     */
    void nodeStatusChanged(const HostAddress& node, NodeCoonectionStatus status);

    /**
     * @brief nodeConfirmend - thim method invocked when the node status changed to "confirmend"
     *  default implementatio do nothing
     * @param node - the address of changed node
     */
    virtual void nodeConfirmend(const HostAddress& node);

    /**
     * @brief nodeConnected thim method invocked when the node status changed to "connected"
     *  default implementatio do nothing
     * @param node
     */
    virtual void nodeConnected(const HostAddress& node);

    /**
     * @brief nodeConnected thim method invocked when the node status changed to "disconnected"
     *  default implementatio do nothing
     * @param node
     */
    virtual void nodeDisconnected(const HostAddress& node);


    /**
     * @brief pushToQueue - This method add action to queue. When the node status will be equal 'triggerStatus' then node run a action method.
     * @param node - node.
     * @param action - lyamda function with action.
     * @param triggerStatus - node status.
     */
    void pushToQueue(const std::function<void ()> &action,
                     const HostAddress& node,
                     NodeCoonectionStatus triggerStatus);

    /**
     * @brief takeFromQueue - take the list of actions of node. after invoke take elements will be removed.
     * @param node - node
     * @param triggerStatus - status of node
     * @return list o actions
     */
    QList<std::function<void ()>> takeFromQueue(const HostAddress& node,
                                                NodeCoonectionStatus triggerStatus);
private slots:

    void avelableBytes();
    void handleDisconnected();
    void handleConnected();
    void handleCheckConfirmendOfNode(HostAddress node);

    /**
     * @brief handleWorkerStoped
     */
    void handleWorkerStoped();

    /**
     * @brief handleForceRemoveNode - force remove connection.
     * @param node
     */
    void handleForceRemoveNode(HostAddress node);

    /**
     * @brief connectNodePrivate
     */
    void connectNodePrivate(NP::HostAddress);

private:

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
    void newWork(const Package &pkg, const AbstractNodeInfo* sender, const HostAddress &id);


    /**
     * @brief nodeConfirmet - this metthod invoked when node is confirment.
     * @param sender - node with new status;
    */
    void nodeConfirmet(const HostAddress &node);

    /**
     * @brief checkConfirmendOfNode - this method remove old not confirmed node.
     * @param node - node address
     */
    void checkConfirmendOfNode(const HostAddress& node);

    SslMode _mode;
    QSslConfiguration _ssl;
    QHash<HostAddress, AbstractNodeInfo*> _connections;
    QHash<HostAddress, ReceiveData> _receiveData;

    QHash<HostAddress, QHash<NodeCoonectionStatus, QList<std::function<void()>>>> _actionCache;

    DataSender * _dataSender = nullptr;

    QSet<QFutureWatcher <bool>*> _workers;

    mutable QMutex _connectionsMutex;
    mutable QMutex _actionCacheMutex;
    mutable QMutex _confirmNodeMutex;

    friend class WebSocketController;


};

}
#endif // ABSTRACTNODE_H
