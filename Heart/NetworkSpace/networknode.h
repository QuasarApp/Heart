/*
 * Copyright (C) 2018-2021 QuasarApp.
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

#include "nodeid.h"

namespace QH {

namespace PKG {
class NetworkMember;
class MemberPermisionObject;
class KnowAddresses;
}

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
     * @param ptr
     */
    NetworkNode(QObject * ptr = nullptr);
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
     * @param localNodeName - this is locale node name. Sets name of folder with the data for current node.
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
     * @param id
     * @return
     */
    bool ping( const NodeId& id);

    /**
     * @brief nodeId
     * @return
     */
    NodeId nodeId() const;

protected:

    /**
     * @brief sendData This implementation of the sendData sending pacakge to the network by node id.
     * @param resp This is sendet pacakge
     * @param nodeId This is id of target node
     * @param req This is header of pacakga that this pacakge ansvered.
     * @return true if the package sendet successful.
     */
    virtual bool sendData(PKG::AbstractData *resp, const NodeId &nodeId,
                  const Header *req = nullptr);

    /**
     * @brief sendData This implementation no prepare data for sendint. for prepare data for sending use the not const object of resp.
     * @param resp This is sendet pacakge
     * @param nodeId This is id of target node
     * @param req This is header of pacakga that this pacakge ansvered.
     * @return true if the package sendet successful.
     */
    virtual bool sendData(const PKG::AbstractData *resp, const NodeId &nodeId,
                  const Header *req = nullptr);

    bool sendData(PKG::AbstractData *resp, const HostAddress &nodeId,
                  const Header *req = nullptr) override;

    bool sendData(const PKG::AbstractData *resp, const HostAddress &nodeId,
                  const Header *req = nullptr) override;

    bool sendData(PKG::AbstractData *resp, const QVariant &nodeId,
                  const Header *req = nullptr) override;

    bool sendData(const PKG::AbstractData *resp, const QVariant &nodeId,
                  const Header *req = nullptr) override;

    void badRequest(const HostAddress &address, const Header &req,
                    const PKG::ErrorData& err, qint8 diff = REQUEST_ERROR) override;

    /**
     * @brief badRequest This implementation of the AbstractNode::badRequest method
     *  send bad request to node with id.
     * @param address This is id of target node or client
     * @param req This is header of request.
     * @param err This is message and code for target node about error. For more onformation see the PKG::ErrorData struct.
     * @param diff This is difference of current trust (currenTrus += diff)
     * By default diff equals REQUEST_ERROR
     */
    virtual void badRequest(const NodeId &address, const Header &req,
                            const PKG::ErrorData& err, qint8 diff = REQUEST_ERROR);


    bool changeTrust(const QVariant &id, int diff) override;
    bool changeTrust(const HostAddress &id, int diff) override;

    QStringList SQLSources() const override;

    /**
     * @brief changeTrust This implementation is some as AbstractNode::changeTrust but change trust of node by id and save changes on local database.
     * @param id This is id of node or client.
     * @param diff This is integer value of trust lavel changes.
     * @return true if functin finished seccussful
     */
    virtual bool changeTrust(const NodeId &id, int diff);

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
    bool savePermision(const PKG::NodeObject &node, const PKG::MemberPermisionObject& permision);
    /**
     * @brief checkSignOfRequest
     * @param request - package
     * @return true if request signed.
     */
    virtual bool checkSignOfRequest(const PKG::AbstractData *request);

    /**
     * @brief thisNode This method return the node object of this node
     * @param returnValue This is return value of nodeobject
     * @return This node object value.
     */
    void thisNode(PKG::NodeObject & returnValue) const;

    /**
     * @brief myKnowAddresses
     * @return set of know addresses
     */
    QSet<NodeId> myKnowAddresses() const;

    bool welcomeAddress(AbstractNodeInfo *node) override;

    /**
     * @brief connectionRegistered - this impletation send incomming node welcom message with information about yaster self.
     * @param info incominng node info.
     */
    void nodeConnected(AbstractNodeInfo *node) override;

    /**
     * @brief nodeConfirmend - this implementation test nodes to double connections
     * @param mode
     */
    void nodeConfirmend(AbstractNodeInfo *sender) override;

    /**
     * @brief nodeDisconnected - this implementation remove nodes info from connection cache
     * @param sender
     */
    void nodeDisconnected(AbstractNodeInfo *node) override;

    /**
     * @brief incomingData - this signal invoked when node get command or ansver
     * @param pkg - received package
     * @param sender - sender of the package
     * @note override this method for get a signals.
     */
    virtual void incomingData(PKG::AbstractData* pkg,
                              const NodeId&  sender);

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
    bool workWithNodeObjectData(const QSharedPointer<PKG::NodeObject> &node, const AbstractNodeInfo *nodeInfo);

    /**
     * @brief workWithKnowAddresses
     * @param node
     * @param nodeInfo
     * @return
     */
    bool workWithKnowAddresses(const PKG::KnowAddresses &obj, const AbstractNodeInfo *nodeInfo);

    /**
     * @brief workWithTransportData
     * @param transportData
     * @param sender
     * @param pkg
     * @return
     */
    ParserResult workWithTransportData(PKG::AbstractData* transportData, const AbstractNodeInfo *sender, const Package &pkg);

    /**
     * @brief workWithNetworkRequest
     * @param networkRequest
     * @param sender
     * @param pkg
     * @return
     */
    ParserResult workWithNetworkRequest(PKG::AbstractData* networkRequest, const AbstractNodeInfo *sender);


    /**
     * @brief optimizeRoute - this method reduces the size of the route by removing unnecessary nodes.
     * @param node
     * @param rawRoute
     * @return
     */
    bool optimizeRoute(const NodeId& node,
                       const HostAddress& currentNodeAddress, const AbstractNodeInfo *sender,
                       QList<HostAddress> rawRoute);

    /**
     * @brief incomingData - this implementation move old incoming method into private section
     *  becouse base node work with BaseID addresses.
     * @warning Do not call this implementation on this class,
     *  use the ncomingData(AbstractData* pkg, const HostAddress&  sender) implementation.
     */
    void incomingData(PKG::AbstractData* pkg,
                      const HostAddress&  sender) override final;


    KeyStorage *_nodeKeys = nullptr;

    Router *_router = nullptr;

    QHash<NodeId, NetworkNodeInfo*> _connections;

    mutable QMutex _connectionsMutex;

};


}
#endif // NETWORKNODE_H
