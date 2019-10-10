#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H

#include "abstractnodeinfo.h"
#include "clientprotocol.h"

#include <QAbstractSocket>
#include <QTcpServer>


namespace ClientProtocol {

/**
 * @brief The NodeInfoData struct
 */
struct NodeInfoData {
    AbstractNodeInfo info;
    BasePackage pkg;
};

#define CRITICAL_ERROOR -50
#define LOGICK_ERROOR   -20
#define REQUEST_ERROR   -5

class AbstractData;

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
    AbstractNode(bool ssl = false, QObject * ptr = nullptr);

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
     * @return info about node
     */
    virtual AbstractNodeInfo *getInfoPtr(quint32 id);
    virtual AbstractNodeInfo getInfo(quint32 id) const;

    /**
     * @brief ban
     * @param target id of ban node
     */
    virtual void ban(quint32 target);

    /**
     * @brief unBan
     * @param target id of unban node
     */
    virtual void unBan(quint32 target);

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



    ~AbstractNode() override;


signals:
    void incomingReques(BasePackage pkg, quint32 sender);


protected:


    /**
     * @brief registerSocket
     * @param socket
     * @return
     */
    virtual bool registerSocket(QAbstractSocket *socket);

    /**
     * @brief parsePackage
     * @param pkg
     * @param sender
     * @return
     */
    virtual bool parsePackage(const BasePackage &pkg, QAbstractSocket * sender);

    /**
     * @brief sendPackage
     * @param pkg
     * @param target
     * @return
     */
    virtual bool sendPackage(const BasePackage &pkg, QAbstractSocket *target);

    /**
     * @brief sendResponse
     * @param resp
     * @param address
     * @param req
     * @return
     */
    virtual bool sendResponse(const AbstractData& resp,  quint32 address,
                      const BaseHeader *req = nullptr);

    /**
     * @brief badRequest
     * @param address
     * @param req
     */
    virtual void badRequest(quint32 address, const BaseHeader &req);

    /**
     * @brief getWorkState
     * @return string of work state
     */
    virtual QString getWorkState() const;

    /**
     * @brief connectionState
     * @return string with count users state
     */
    virtual QString connectionState() const;

    /**
     * @brief baned
     * @return
     */
    QStringList baned() const;

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

protected:
    bool changeTrust(quint32 id, int diff);


private slots:
    void incommingData();
    void avelableBytes();
    void handleDisconnected();

private:
    bool useSSL = false;
    QHash<unsigned int, NodeInfoData> _connections;


};
}
#endif // ABSTRACTNODE_H
