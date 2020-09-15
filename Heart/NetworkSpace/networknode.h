/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <databasenode.h>
#include <dbobject.h>
#include <hostaddress.h>
#include <nodeobject.h>


namespace QH {

class NetworkMember;
class MemberPermisionObject;
class KnowAddresses;
class KeyStorage;
class Router;
class NetworkNodeInfo;

/**
 * @brief The BaseNode class - base inplementation of nodes. This implementation contains methods for work with database and work with data transopt on network.
 *  BaseNode - is thread save class
 */
class HEARTSHARED_EXPORT NetworkNode : public DataBaseNode
{
    Q_OBJECT
public:

    /**
     * @brief BaseNode
     * @param mode
     * @param ptr
     */
    NetworkNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);
    ~NetworkNode() override;

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
    bool run(const QString &addres, unsigned short port,
                     const QString &localNodeName) override;

    /**
     * @brief stop - this implementation stop work database and push to database all cache data.
     */
    void stop() override;

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

protected:

    /**
     * @brief sendDataToId - send data to node or clientby them id.
     * @param resp - responce package
     * @param nodeId - id of target node
     * @param req - header of request
     * @return true if data sendet seccussful
     */
    bool sendData(const AbstractData *resp, const BaseId &nodeId,
                          const Header *req = nullptr) override;

    bool sendData(AbstractData *resp, const BaseId &nodeId,
                          const Header *req = nullptr) override;
    bool sendData(const AbstractData *resp, const HostAddress &nodeId,
                          const Header *req = nullptr) override;
    bool sendData(AbstractData *resp, const HostAddress &nodeId,
                          const Header *req = nullptr) override;

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
     * @brief createNodeInfo - this method create a new node from socket. override this mehod if you want to create a own clasess of nodes.
     * @param socket
     * @return pointer to new node info
     */
    AbstractNodeInfo* createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const override;

    /**
     * @brief savePermision - this method save a new changes in to permisions table.
     *  Befor save new data node can be validate new data with consensus.
     * @param permision - data of new permision
     * @return true if new cghanges saved successful.
     */
    bool savePermision(const NodeObject &node, const MemberPermisionObject& permision);
    /**
     * @brief checkSignOfRequest
     * @param request - package
     * @return true if request signed.
     */
    virtual bool checkSignOfRequest(const AbstractData *request);

    /**
     * @brief thisNode
     * @return This node object value.
     */
    NodeObject thisNode() const;

    /**
     * @brief myKnowAddresses
     * @return set of know addresses
     */
    QSet<BaseId> myKnowAddresses() const;

    /**
     * @brief welcomeAddress - this method send to the ip information about yaster self.
     * @param ip - host address of the peer node obeject
     * @return true if all iformation sendet succesful
     */
    bool welcomeAddress(const HostAddress &ip) override;

    /**
     * @brief connectionRegistered - this impletation send incomming node welcom message with information about yaster self.
     * @param info incominng node info.
     */
    void nodeConnected(const HostAddress& node) override;

    /**
     * @brief nodeConfirmend - this implementation test nodes to double connections
     * @param mode
     */
    void nodeConfirmend(const HostAddress& sender) override;

    /**
     * @brief nodeDisconnected - this implementation remove nodes info from connection cache
     * @param sender
     */
    void nodeDisconnected(const HostAddress& node) override;

    /**
     * @brief incomingData - this signal invoked when node get command or ansver
     * @param pkg - received package
     * @param sender - sender of the package
     * @note override this method for get a signals.
     */
    virtual void incomingData(AbstractData* pkg,
                              const BaseId&  sender);

    /**
     * @brief keyStorageLocation - return location of storagge of keys.
     * @return path to the location of keys storage
     */
    QString keyStorageLocation() const;

private:

    /**
     * @brief workWithNodeObjectData - this method working with received node object data.
     * @param node
     * @param nodeInfo
     * @return true if function finished successful
     */
    bool workWithNodeObjectData(NodeObject &node, const AbstractNodeInfo *nodeInfo);

    /**
     * @brief workWithKnowAddresses
     * @param node
     * @param nodeInfo
     * @return
     */
    bool workWithKnowAddresses(const KnowAddresses &obj, const AbstractNodeInfo *nodeInfo);

    /**
     * @brief workWithTransportData
     * @param transportData
     * @param sender
     * @param pkg
     * @return
     */
    ParserResult workWithTransportData(AbstractData* transportData, const AbstractNodeInfo *sender, const Package &pkg);

    /**
     * @brief workWithNetworkRequest
     * @param networkRequest
     * @param sender
     * @param pkg
     * @return
     */
    ParserResult workWithNetworkRequest(AbstractData* networkRequest, const AbstractNodeInfo *sender);


    /**
     * @brief optimizeRoute - this method reduces the size of the route by removing unnecessary nodes.
     * @param node
     * @param rawRoute
     * @return
     */
    bool optimizeRoute(const BaseId& node,
                       const HostAddress& currentNodeAddress, const AbstractNodeInfo *sender,
                       QList<HostAddress> rawRoute);

    /**
     * @brief incomingData - this implementation move old incoming method into private section
     *  becouse base node work with BaseID addresses.
     * @warning Do not call this implementation on this class,
     *  use the ncomingData(AbstractData* pkg, const HostAddress&  sender) implementation.
     */
    void incomingData(AbstractData* pkg,
                      const HostAddress&  sender) override final;


    KeyStorage *_nodeKeys = nullptr;

    Router *_router = nullptr;

    QHash<BaseId, NetworkNodeInfo*> _connections;

    mutable QMutex _connectionsMutex;

};


}
#endif // NETWORKNODE_H
