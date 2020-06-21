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
class DBDataRequest;


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

    ~BaseNode() override;

    /**
     * @brief defaultDbParams
     * @return
     */
    virtual QVariantMap defaultDbParams() const;

    /**
     * @brief sendDataToId - send data to node or clientby them id
     * @param resp - responce package
     * @param nodeId - id of target node
     * @param req - header of request
     * @return true if data sendet seccussful
     */
    virtual bool sendDataToId(const AbstractData* resp, const QByteArray& nodeId,
                              const Header *req = nullptr);
signals:
    void incomingData(AbstractData* pkg,
                      const QHostAddress&  sender);

    void requestError(QString msg);

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
    bool workWithSubscribe(const AbstractData* rec,
                           const QHostAddress &address);


    /**
     * @brief checkPermision - check permison of object for selected node
     * @param requestNode - node
     * @param objcet - address of required object
     * @param requiredPermision - required permision
     * @return true if the node have required permison for selected object
     */
    virtual DBOperationResult checkPermision(const AbstractNodeInfo *requestNode,
                                             const DbAddress& object,
                                             const int &requiredPermision);



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
     * @brief deleteObject - delete objcet from dataBase
     * @param rec
     * @param addere
     * @return operation status
     */
    DBOperationResult deleteObject(const AbstractData *rec,
                                   const QHostAddress &addere);

    /**
     * @brief getObject - general object for get object
     * @param res - result object
     * @param requiredNodeAdderess
     * @param dbObject
     * @return operation status
     */
    DBOperationResult getObject(DBObject* res,
                                const QHostAddress &requiredNodeAdderess,
                                const DbAddress& dbObject);


    /**
     * @brief setObject
     * @param saveObject
     * @param requiredNodeAddere
     * @param dbObject
     * @return operation status
     */
    DBOperationResult setObject(const DBObject* saveObject,
                                const QHostAddress &requiredNodeAddere,
                                const DbAddress& dbObject);


private:
    SqlDBCache *_db = nullptr;
    ICrypto *_nodeKeys = nullptr;

    /**
     * @brief workWithAvailableDataRequest
     * @param rec
     * @param addere
     * @param rHeader
     * @return
     */
    bool workWithAvailableDataRequest(const AbstractData *rec,
                                      const QHostAddress &addere,
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
