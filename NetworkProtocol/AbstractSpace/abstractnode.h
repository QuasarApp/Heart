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
#include <QSslConfiguration>
#include <QTcpServer>
#include "abstractdata.h"
#include "workstate.h"

class QSslCertificate;
class QSslKey;
class QSslConfiguration;

#include "cryptopairkeys.h"
#include "icrypto.h"
#include "networkprotocol_global.h"

namespace NP {

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
 * @brief The NodeInfoData struct
 */
struct NodeInfoData {
    AbstractNodeInfo *info = nullptr;
    Package pkg;
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
 * @brief The AbstractNode class
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
     * @brief stop stop this node
     */
    virtual void stop();

    /**
     * @brief getInfo
     * @param id of selected node
     * @return pointer to information about node. if address not found return nullpt
     */
    virtual AbstractNodeInfo* getInfoPtr(const QHostAddress &id);

    /**
     * @brief getInfoPtr
     * @param id peer adders
     * @return pointer to information about node. if address not found return nullpt
     */
    virtual const AbstractNodeInfo* getInfoPtr(const QHostAddress &id) const;

    /**
     * @brief ban
     * @param target id of ban node
     */
    virtual void ban(const QHostAddress& target);

    /**
     * @brief unBan
     * @param target id of unban node
     */
    virtual void unBan(const QHostAddress& target);

    /**
     * @brief connectToHost - connect to host node
     * @param ip address of node
     * @param port - port of node
     * @param mode - mode see SslMode
     */
    virtual void connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode = SslMode::NoSSL);

    /**
     * @brief connectToHost - connect to host node. this method find ip address of domain befor connecting
     * @param domain: address of node
     * @param port - port of node
     * @param mode - mode see SslMode
     */
    virtual void connectToHost(const QString &domain, unsigned short port, SslMode mode = SslMode::NoSSL);

    /**
     * @brief addNode - add new node for connect
     * @param nodeAdderess - the network addres of a new node.
     * @param port - port of node
     */
    void addNode(const QHostAddress& nodeAdderess, int port);

    /**
     * @brief removeNode - remove node
     * @param nodeAdderess - the adddress of removed node.
     * @param port - port of node
     */
    void removeNode(const QHostAddress& nodeAdderess, int port);

    /**
     * @brief port
     * @return current node port
     */
    unsigned short port() const;

    /**
     * @brief address
     * @return return current adders
     */
    QHostAddress address() const;

    /**
     * @brief getSslConfig
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
     * @brief nodeId
     * @return signed publicKey;
     */
    QByteArray nodeId() const;

    /**
     * @brief checkNodeId
     * @return
     */
    bool checkNodeId(const QByteArray& nodeId) const;

    /**
     * @brief connectionsCount
     * @return
     */
    int connectionsCount() const;

    /**
     * @brief ping - ping address for testing
     * @param address - address of other node
     * @return true if ping sendet
     */
    bool ping( const QHostAddress& address);

    /**
     * @brief getKnowedNodes
     * @return the set of konowed nodes.
     */
    const QHash<QHostAddress, int> &getKnowedNodes() const;

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
    virtual AbstractNodeInfo* createNodeInfo(QAbstractSocket *socket) const;

    /**
     * @brief registerSocket
     * @param socket
     * @return
     */
    virtual bool registerSocket(QAbstractSocket *socket,
                                const QHostAddress *clientAddress = nullptr);

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
     * @brief sendData send data package to address
     * @param resp
     * @param address
     * @param req
     * @return
     */
    virtual bool sendData(const AbstractData* resp,  const QHostAddress& addere,
                              const Header *req = nullptr) const;

    /**
     * @brief badRequestu
     * @param address
     * @param req
     * @param msg - message of error
     */
    virtual void badRequest(const QHostAddress &address, const Header &req,
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
    QList<QHostAddress> banedList() const;

    /**
     * @brief isBaned
     * @param socket
     * @return
     */
    bool isBaned(QAbstractSocket* socket) const;

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
    virtual bool changeTrust(const QHostAddress& id, int diff);

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
                      const QHostAddress&  sender);

    /**
     * @brief connections - return hash map of all connections of this node.
     * @return
     */
    const QHash<QHostAddress, NodeInfoData>& connections() const;
private slots:

    void avelableBytes();
    void handleDisconnected();


private:

    /**
     * @brief reconnectAllKonowedNodes
     */
    void reconnectAllKonowedNodes();

    SslMode _mode;
    QSslConfiguration _ssl;
    QHash<QHostAddress, NodeInfoData> _connections;
    QMultiHash<QHostAddress, int> _knowedNodes;

    friend class WebSocketController;


};
}
#endif // ABSTRACTNODE_H
