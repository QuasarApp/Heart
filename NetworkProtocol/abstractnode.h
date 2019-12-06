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
#include "defines.h"

class QSslCertificate;
class QSslKey;
class QSslConfiguration;

namespace NP {

/**
 * @brief The ParserResult enum
 * Error - parser detect a errorob package
 * NotProcessed - the parser does not know what to do with the package or has not finished processing it.
 * Processed - the parser finished processing correctly
 */
enum class ParserResult {
    Error = 0,
    NotProcessed = 1,
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
    SP<AbstractNodeInfo> info;
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
class AbstractNode : public QTcpServer
{
    Q_OBJECT

public:

    /**
     * @brief AbstractNode
     * @param ssl
     * @param ptr
     */
    AbstractNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);

    /**
     * @brief run
     * @param addres
     * @param port
     * @return
     */
    virtual bool run(const QString& addres, unsigned short port);

    /**
     * @brief stop stop this node
     */
    virtual void stop();

    /**
     * @brief getInfo
     * @param id of selected node
     * @return pointer to information about node
     */
    virtual WP<AbstractNodeInfo> getInfoPtr(const QHostAddress &id);

    /**
     * @brief getInfo
     * @param id peer adders
     * @return information about Node
     */
    virtual AbstractNodeInfo getInfo(const QHostAddress &id) const;

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
     * @brief connectToHost
     */
    virtual void connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode = SslMode::NoSSL);

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

    ~AbstractNode() override;

    SslMode getMode() const;

    /**
     * @brief getWorkState
     * @return
     */
    virtual WorkState getWorkState() const;


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
    virtual SP<AbstractNodeInfo> createNodeInfo(QAbstractSocket *socket) const;

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
    virtual ParserResult parsePackage(const Package &pkg, const WP<AbstractNodeInfo> &sender);

    /**
     * @brief sendPackage
     * @param pkg
     * @param target
     * @return
     */
    virtual bool sendPackage(const Package &pkg, QAbstractSocket *target);

    /**
     * @brief sendResponse
     * @param resp
     * @param address
     * @param req
     * @return
     */
    virtual bool sendData(const WP<AbstractData> &resp,  const QHostAddress& addere,
                              const Header *req = nullptr);

    /**
     * @brief badRequest
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
     * @brief connectionsCount
     * @return
     */
    int connectionsCount() const;

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
    bool changeTrust(const QHostAddress& id, int diff);

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


private slots:

    void avelableBytes();
    void handleDisconnected();


private:
    SslMode _mode;
    QSslConfiguration _ssl;
    QHash<QHostAddress, NodeInfoData> _connections;

    friend class WebSocketController;

};
}
#endif // ABSTRACTNODE_H
