/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DATABASENODE_H
#define DATABASENODE_H

#include "abstractnode.h"
#include <dbobject.h>
#include <hostaddress.h>
#include <permission.h>


namespace QH {

namespace PKG {
class WebSocket;

}

class SqlDBCache;
class SqlDBWriter;
class WebSocketController;
class DbAddress;
class BaseId;


/**
 * @brief The BaseNode class - base inplementation of nodes. This implementation contains methods for work with database and work with data transopt on network.
 *  BaseNode - is thread save class
 */
class HEARTSHARED_EXPORT DataBaseNode : public AbstractNode
{
    Q_OBJECT
public:

    /**
     * @brief BaseNode
     * @param ptr
     */
    DataBaseNode(QObject * ptr = nullptr);
    ~DataBaseNode() override;

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

    /**
     * @brief stop - this implementation stop work database and push to database all cache data.
     */
    void stop() override;



    /**
     * @brief defaultDbParams
     * @return
     */
    virtual QVariantMap defaultDbParams() const;

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
     * @brief sendData - send data to an ip address
     * @param resp
     * @param addere
     * @param req
     * @return true if a function finished seccussful
     */
    bool sendData(PKG::AbstractData *resp,
                  const HostAddress &addere,
                  const Header *req = nullptr) override;

    /**
     * @brief sendData - send data to an ip address
     * @param resp
     * @param addere
     * @param req
     * @return true if a function finished seccussful
     */
    bool sendData(const PKG::AbstractData *resp,
                  const HostAddress &addere,
                  const Header *req = nullptr) override;

    /**
     * @brief sendDataToId - send data to node or clientby them id. This implementation prepare object to sending.
     * @param resp - responce package
     * @param nodeId - id of target node
     * @param req - header of request
     * @return true if data sendet seccussful
     */
    virtual bool sendData(PKG::AbstractData *resp, const BaseId &nodeId,
                          const Header *req = nullptr);

    AbstractNodeInfo *createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const override;

    /**
     * @brief sendDataToId - send data to node or clientby them id.
     * @param resp - responce package
     * @param nodeId - id of target node
     * @param req - header of request
     * @return true if data sendet seccussful
     */
    virtual bool sendData(const PKG::AbstractData *resp, const BaseId &nodeId,
                          const Header *req = nullptr);
    /**
     * @brief badRequest -send bad request and change trus for ip address
     * @param address
     * @param req
     * @param msg
     */
    void badRequest(const HostAddress &address, const Header &req,
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
    bool changeTrust(const HostAddress &id, int diff) override;

    /**
     * @brief changeTrust change trus of node with id.
     * @param id
     * @param diff
     * @return true if functin finished seccussful
     */
    virtual bool changeTrust(const BaseId &id, int diff);

    /**
     * @brief hashgenerator
     * @param pass
     */
    virtual QString hashgenerator(const QByteArray &pass);


    /**
     * @brief nodeConnected - this implementation call a welcomeAddress method.
     * @param node
     */
    void nodeConnected(const HostAddress &node) override;

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
    bool workWithSubscribe(const PKG::WebSocket &rec,
                           const BaseId &clientOrNodeid, const AbstractNodeInfo &sender);

    /**
     * @brief deleteObject - delete object by address dbObject
     * @param requester - reqester.
     * @param dbObject
     * @return result of operation (allow, forbiden unknown)
     */
    DBOperationResult deleteObject(const BaseId &requester,
                                   const PKG::DBObject *dbObject);

    /**
     * @brief getObject - general method for geting object wth check permisions
     *  this function check permishen to requested object and set new object to res if access granted.
     * @param requester - requser  node or client
     * @param templateObj - object with request
     * @param result - pointer to pointer of result object
     * @return operation status
     */
    DBOperationResult getObject(const BaseId &requester,
                                const PKG::DBObject &templateObj,
                                const PKG::DBObject **result) const;


    /**
     * @brief getObjects - general object for get object
     *  this function check permishen to requested object and set new object to res if access granted.
     * @param requester - requser  node or client
     * @param templateObj - object with request
     * @param result - pointer to list of pointers with result objects
     * @return operation status
     */
    DBOperationResult getObjects(const BaseId &requester,
                                 const PKG::DBObject &templateObj,
                                 QList<const PKG::DBObject *> *result) const;

    /**
     * @brief setObject
     * @param saveObject
     * @param requiredNodeAddere
     * @param dbObject
     * @return operation status
     */
    DBOperationResult setObject(const BaseId &requester,
                                const PKG::DBObject *saveObject);

    /**
     * @brief getSender - this method return id of requester.
     *  @default Base implementation get id from BaseNdoeInfo.
     * @param connectInfo - info about connect
     * @param requestData - data of request
     * @return id of requester member
     */
    virtual BaseId getSender(const AbstractNodeInfo *connectInfo, const PKG::AbstractData *requestData) const;

    /**
     * @brief checkPermision - check permision of requester to objectAddress
     *  Override this method for your backend
     * @param requester - user on node or another object of network
     * @param objectAddress - addres to database object
     * @param requarimentPermision - needed permision for requester
     * @return DBOperationResult::Alowed if permission granted
     */
    virtual DBOperationResult checkPermission(const BaseId& requester,
                                const DbAddress& objectAddress,
                                const Permission& requarimentPermision) const;

    /**
     * @brief dbLocation - return location of database of node.
     * @return path to the location of database
     */
    QString dbLocation() const;

    /**
     * @brief welcomeAddress - this method send to the ip information about yaster self.
     * @param ip - host address of the peer node obeject
     * @return true if all iformation sendet succesful
     */
    virtual bool welcomeAddress(const HostAddress &ip);

    /**
     * @brief isBanned - check trust of node, if node trus is lover of 0 return true.
     * @param member
     * @return true if node is banned
     */
    bool isBanned(const BaseId& member) const;
private:

    SqlDBCache *_db = nullptr;
    QString _localNodeName;
    WebSocketController *_webSocketWorker = nullptr;


    friend class WebSocketController;

};


}
#endif // DATABASENODE_H
