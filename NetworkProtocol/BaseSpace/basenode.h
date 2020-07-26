/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef BASENODE_H
#define BASENODE_H

#include "abstractnode.h"
#include <dbobject.h>

namespace NP {

class SqlDBCache;
class SqlDBWriter;
class UserData;
class UserRequest;
class AvailableDataRequest;
class WebSocket;
class WebSocketController;
class DbAddress;
class BaseId;
class Sign;
class KeyStorage;

/**
 * @brief The BaseNode class - base inplementation of nodes
 */
class NETWORKPROTOCOLSHARED_EXPORT BaseNode : public AbstractNode
{
    Q_OBJECT
public:

    /**
     * @brief BaseNode
     * @param mode
     * @param ptr
     */
    BaseNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);

    /**
     * @brief intSqlDb - this function init database of node
     * @param DBparamsFile - path to json file with all patarams
     * @param cache - new SqlDBCache object
     * @param writer - new SqlDBWriter
     * @return true if all good
     */
    virtual bool initSqlDb( QString DBparamsFile = "",
                            SqlDBCache * cache = nullptr,
                            SqlDBWriter* writer = nullptr);

    /**
     * @brief isSqlInited
     * @return return true if intSqlDb invocked correctly;
     */
    bool isSqlInited() const;


    /**
     * @brief run server on address an port
     * @param addres - If address is empty then serve weel be listen all addreses of all interfaces
     * @param port
     * @return recomendet befor invoke this method call the intSqlDb.
     * If you skeap a call of intSqlDb method then data base inited with default parameters.
     */
    bool run(const QString &addres, unsigned short port) override;

    /**
     * @brief run server on address an port with local name of storage of keys
     * @param addres - network address of node
     * @param port - port of node
     * @return true if node is deployed successful
     */
    virtual bool run(const QString &addres, unsigned short port,
                     const QString &localNodeName);

    ~BaseNode() override;

    /**
     * @brief defaultDbParams
     * @return
     */
    virtual QVariantMap defaultDbParams() const;

    /**
     * @brief sendData - send data to an ip address
     * @param resp
     * @param addere
     * @param req
     * @return true if a function finished seccussful
     */
    bool sendData(const AbstractData *resp,
                  const QHostAddress &addere,
                  const Header *req = nullptr) const override;

    /**
     * @brief sendDataToId - send data to node or clientby them id
     * @param resp - responce package
     * @param nodeId - id of target node
     * @param req - header of request
     * @return true if data sendet seccussful
     */
    virtual bool sendData(const AbstractData *resp, const BaseId &nodeId,
                          const Header *req = nullptr) const;

    /**
     * @brief badRequest -send bad request and change trus for ip address
     * @param address
     * @param req
     * @param msg
     */
    void badRequest(const QHostAddress &address, const Header &req,
                    const QString msg = "") override;

    /**
     * @brief badRequest - send bad request to node with id
     * @param address - id of target node or client
     * @param req - header of an accepted request.
     * @param msg
     */
    virtual void badRequest(const BaseId &address, const Header &req,
                            const QString msg = "");

    /**
     * @brief changeTrust - change trust of ip address
     * @param id - ip address of node
     * @param diff
     */
    bool changeTrust(const QHostAddress &id, int diff) override;

    /**
     * @brief changeTrust change trus of node with id.
     * @param id
     * @param diff
     * @return true if functin finished seccussful
     */
    virtual bool changeTrust(const BaseId &id, int diff);

    /**
     * @brief ping - ping node by node id
     * @param address
     * @return
     */
    bool ping( const BaseId& id);

    /**
     * @brief nodeId
     * @return
     */
    BaseId nodeId() const;

    /**
     * @brief connectToHost - this ovverided method diference betwn base implementation then it send a request for get node id.
     * @param ip
     * @param port
     * @param mode
     */
    void connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode) override;



protected:


    /**
     * @brief initDefaultDbObjects create default cache and db writer if pointer is null
     * @param cache
     * @param writer
     */
    void initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer);

    /**
     * @brief parsePackage
     * @param pkg
     * @param sender
     * @return
     */
    ParserResult parsePackage(const Package &pkg,
                              const AbstractNodeInfo* sender) override;

    /**
     * @brief hashgenerator
     * @param pass
     */
    virtual QString hashgenerator(const QByteArray &pass);

    /**
     * @brief createNodeInfo - this method create a new node from socket. override this mehod if you want to create a own clasess of nodes.
     * @param socket
     * @return pointer to new node info
     */
    AbstractNodeInfo* createNodeInfo(QAbstractSocket *socket) const override;

    /**
     * @brief db
     * @return pinter to data base
     */
    SqlDBCache* db() const;

    /**
     * @brief workWithSubscribe - this metod work work with subscribe commnads
     * @param rec request data
     * @param address sendet address
     * @return true if data parsed seccusseful
     */
    bool workWithSubscribe(const WebSocket &rec,
                           const BaseId &clientOrNodeid);

    template<class RequestobjectType>
    /**
     * @brief workWithDataRequest
     * @param rec
     * @param addere
     * @param rHeader
     * @return
     */
    bool workWithDataRequest(const AbstractData* rec,
                             const QHostAddress &addere,
                             const Header *rHeader);

    /**
     * @brief deleteObject - delete object by address dbObject
     * @param requester - reqester.
     * @param dbObject
     * @return result of operation (allow, forbiden unknown)
     */
    DBOperationResult deleteObject(const BaseId &requester,
                                   const DBObject *dbObject);

    /**
     * @brief getObject - general method for geting object wth check permisions
     *  this function check permishen to requested object and set new object to res if access granted.
     * @param requester - requser  node or client
     * @param templateObj - object with request
     * @param result - pointer to pointer of result object
     * @return operation status
     */
    DBOperationResult getObject(const BaseId &requester,
                                const DBObject &templateObj,
                                DBObject **result) const;


    /**
     * @brief getObjects - general object for get object
     *  this function check permishen to requested object and set new object to res if access granted.
     * @param requester - requser  node or client
     * @param templateObj - object with request
     * @param result - pointer to list of pointers with result objects
     * @return operation status
     */
    DBOperationResult getObjects(const BaseId &requester,
                                 const DBObject &templateObj,
                                 QList<DBObject *> *result) const;

    /**
     * @brief setObject
     * @param saveObject
     * @param requiredNodeAddere
     * @param dbObject
     * @return operation status
     */
    DBOperationResult setObject(const BaseId &requester,
                                const DBObject *saveObject);

    /**
     * @brief checkSignOfRequest
     * @param request - package
     * @return true if request signed.
     */
    virtual bool checkSignOfRequest(const AbstractData *request);

private:
    SqlDBCache *_db = nullptr;
    KeyStorage *_nodeKeys = nullptr;
    QString _localNodeName;

    /**
     * @brief workWithAvailableDataRequest
     * @param rec
     * @param addere
     * @param rHeader
     * @return
     */
    bool workWithAvailableDataRequest(const AvailableDataRequest &rec,
                                      const Header *rHeader);


    WebSocketController *_webSocketWorker = nullptr;

    bool workWithUserRequest(const QSharedPointer<UserRequest> &request,
                             const QHostAddress &addere,
                             const Header *rHeader);

    bool loginUser(const QWeakPointer<AbstractData> &user,
                   const QWeakPointer<AbstractData> &userdb,
                   const QHostAddress &address);

    bool registerNewUser(const QWeakPointer<AbstractData> &user,
                         const QHostAddress &address,
                         bool rememberMe);

};


}
#endif // BASENODE_H
