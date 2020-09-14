/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "networknode.h"
#include "basenodeinfo.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"
#include "badnoderequest.h"
#include "sign.h"
#include "asyncsqldbwriter.h"
#include "router.h"

#include <badrequest.h>
#include <quasarapp.h>
#include <transportdata.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <websocketcontroller.h>
#include <QCoreApplication>
#include <qsecretrsa2048.h>
#include <ping.h>
#include <keystorage.h>
#include <knowaddresses.h>
#include <longping.h>
#include <networkrequest.h>
#include <networkmember.h>
#include <networknodeinfo.h>


#define THIS_NODE "this_node_key"
namespace NP {

NetworkNode::NetworkNode(NP::SslMode mode, QObject *ptr):
    DataBaseNode(mode, ptr) {

    _nodeKeys = new KeyStorage(new QSecretRSA2048());
    _router = new Router();

}

bool NetworkNode::run(const QString &addres, unsigned short port) {
    if (!DataBaseNode::run(addres, port)) {
        return false;
    }

    _nodeKeys->initDefaultStorageLocation();
    _nodeKeys->start();

    return true;
}

bool NetworkNode::run(const QString &addres,
                   unsigned short port,
                   const QString &localNodeName) {

    if (!DataBaseNode::run(addres, port, localNodeName)) {
        return false;
    }

    _nodeKeys->initDefaultStorageLocation(localNodeName);
    _nodeKeys->start();

    return true;
}

void NetworkNode::stop() {
    DataBaseNode::stop();

}

NetworkNode::~NetworkNode() {
    delete _nodeKeys;
    delete _router;
}

BaseId NetworkNode::nodeId() const {

    auto keys = _nodeKeys->getNextPair(THIS_NODE);
    return QCryptographicHash::hash(keys.publicKey(), QCryptographicHash::Sha256);
}

bool NetworkNode::checkSignOfRequest(const AbstractData *request) {
    auto object = dynamic_cast<const Sign*>(request);
    auto dbObject = dynamic_cast<const SenderData*>(request);

    if (!(object && dbObject)) {
        return false;
    }

    auto node = db()->getObject(NetworkMember{dbObject->senderID()});
    return _nodeKeys->check(_nodeKeys->concatSign(object->dataForSigned(),
                                                  object->sign()), node->authenticationData());
}

NetworkMember NetworkNode::thisNode() const {
    NetworkMember res;
    auto keys = _nodeKeys->getNextPair(THIS_NODE);

    res.setAuthenticationData(keys.publicKey());
    res.setTrust(0);

    res.prepareToSend();

    return res;
}

QSet<BaseId> NetworkNode::myKnowAddresses() const {
    QSet<BaseId> res;

    for (const AbstractNodeInfo *i : connections()) {
        auto info = dynamic_cast<const BaseNodeInfo*>(i);
        if (info && info->selfId().isValid()) {
            res += info->selfId();
        }
    }

    return res;
}

bool NetworkNode::welcomeAddress(const HostAddress& ip) {
    NetworkMember self = thisNode();

    if (!sendData(&self, ip)) {
        return false;
    }

    if (connections().size()) {

        auto knowAddresses = myKnowAddresses();
        if (knowAddresses.size()) {
            KnowAddresses addressesData;
            addressesData.setKnowAddresses(knowAddresses);

            if (!sendData(&addressesData, ip)) {
                return false;
            }
        }
    }

    return true;

}

void NetworkNode::nodeConnected(const HostAddress &node) {
    DataBaseNode::nodeConnected(node);

    welcomeAddress(node);

}

void NetworkNode::nodeConfirmend(const HostAddress &node) {
    DataBaseNode::nodeConfirmend(node);

    auto nodeInfo = dynamic_cast<NetworkNodeInfo*>(getInfoPtr(node));
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    NetworkNodeInfo* oldNodeInfo = _connections.value(nodeInfo->selfId(), nullptr);
    _connectionsMutex.unlock();

    if (oldNodeInfo) {
        removeNode(node);
        return;
    }

    _connectionsMutex.lock();
    _connections.insert(nodeInfo->selfId(), nodeInfo);

    _connectionsMutex.unlock();

}

void NetworkNode::nodeDisconnected(const HostAddress &node) {
    AbstractNode::nodeDisconnected(node);

    auto nodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(node));
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    _connections.remove(nodeInfo->selfId());
    _connectionsMutex.unlock();
}

void NetworkNode::incomingData(AbstractData *, const BaseId &) {}

QString NetworkNode::keyStorageLocation() const {
    return _nodeKeys->storageLocation();
}

ParserResult NetworkNode::parsePackage(const Package &pkg,
                                    const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto baseSender = dynamic_cast<const BaseNodeInfo*>(sender);
    if (!baseSender) {
        QuasarAppUtils::Params::log("Sender is not basenode info!",
                                    QuasarAppUtils::Error);
        return ParserResult::Error;
    }

    if (H_16<BadNodeRequest>() == pkg.hdr.command) {
        BadNodeRequest cmd(pkg);

        incomingData(&cmd, baseSender->selfId());
        emit requestError(cmd.err());

        return ParserResult::Processed;

    } else if (H_16<TransportData>() == pkg.hdr.command) {
        TransportData cmd(pkg);
        return workWithTransportData(&cmd, sender, pkg);


    } else if (H_16<NetworkMember>() == pkg.hdr.command) {
        NetworkMember obj(pkg);
        if (!obj.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithNodeObjectData(obj, sender)) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;
    } else if (H_16<KnowAddresses>() == pkg.hdr.command) {
        KnowAddresses obj(pkg);
        if (!obj.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithKnowAddresses(obj, sender)) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;
    } else if (H_16<LongPing>() == pkg.hdr.command) {
        LongPing cmd(pkg);

        if (!cmd.ansver()) {
            cmd.setAnsver(true);
            sendData(&cmd, cmd.senderID(), &pkg.hdr);
        }

        incomingData(&cmd, baseSender->selfId());
        return ParserResult::Processed;
    } else if (H_16<NetworkRequest>() == pkg.hdr.command) {
        NetworkRequest cmd(pkg);

        if (!cmd.isValid()) {
            badRequest(baseSender->selfId(), pkg.hdr);
            return ParserResult::Error;
        }

        workWithNetworkRequest(&cmd, baseSender);
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;
}

bool NetworkNode::workWithAvailableDataRequest(const AvailableDataRequest &rec,
                                            const Header *rHeader) {
    if (!_db)
        return false;

    if (!checkSignOfRequest(&rec)) {
        return false;
    }

    auto availableData = _db->getObject(rec);
    if (!availableData)
        return false;

    //To-Do brodcast yhis request to all network

    return sendData(availableData, rec.senderID(), rHeader);

}

bool NetworkNode::workWithNodeObjectData(NetworkMember &node,
                                      const AbstractNodeInfo* nodeInfo) {

    if (!_db) {
        return false;
    }

    auto localObjec = _db->getObject(node);

    if (localObjec) {
        node.setTrust(std::min(node.trust(), localObjec->trust()));
    } else {
        node.setTrust(0);
    }

    if (!db()->saveObject(&node)) {
        return false;
    };

    auto peerNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->setSelfId(node.getId());

    return true;
}

bool NetworkNode::workWithKnowAddresses(const KnowAddresses &obj,
                                     const AbstractNodeInfo *nodeInfo) {

    auto peerNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->addKnowAddresses(obj.knowAddresses());

    return true;
}

ParserResult NetworkNode::workWithTransportData(AbstractData *transportData,
                                             const AbstractNodeInfo* sender,
                                             const Package& pkg) {

    // convert transoprt pakcage
    auto cmd = dynamic_cast<TransportData*>(transportData);

    if (!cmd)
        return ParserResult::Error;

    // ignore processed packages
    if (_router->isProcessed(cmd->packageId()))
        return ParserResult::Processed;

    // check distanation
    if (cmd->targetAddress() == nodeId()) {

        // inversion route and update route to sender
        _router->updateRoute(cmd->senderID(),
        {cmd->route().rbegin(), cmd->route().rend()});

        return parsePackage(cmd->data(), sender);
    }

    bool fRouteIsValid = false;

    // find route if transport command do not have own route.
    if (!cmd->isHaveRoute() && _router->contains(cmd->targetAddress())) {
        auto route = _router->getRoute(cmd->targetAddress());

        auto cmdRute = cmd->route();

        cmdRute.push_back(address());
        cmdRute.append(route);

        cmd->setRoute(cmdRute);
    }

    // check exists route
    if (cmd->isHaveRoute()) {

        // if package have a route then remove all nodes from sender to this node from route and update route
        if (!sender)
            return ParserResult::Error;

        cmd->strip(sender->networkAddress(), address());

        // send this package to first available node of knownnodes route nodes
        auto it = cmd->route().rbegin();
        while (it != cmd->route().rend() && !sendData(cmd, *it, &pkg.hdr)) {
            it++;
        }

        fRouteIsValid = it != cmd->route().rend();

    } else {
        // if command not have a route or route is not completed then add this node to end route
        cmd->addNodeToRoute(address());
    }

    // save route for sender node from this node
    auto routeFromSenderToHere = cmd->route();
    int index = routeFromSenderToHere.indexOf(address());

    if (index < 0) {
        QuasarAppUtils::Params::log("own node no findet on route.",
                                    QuasarAppUtils::Error);
    }

    routeFromSenderToHere.erase(routeFromSenderToHere.begin(), routeFromSenderToHere.begin() + index);
    // inversion route and update route to sender
    _router->updateRoute(cmd->senderID(),
    {routeFromSenderToHere.rbegin(), routeFromSenderToHere.rend()});

    // remove invalid route nodes and fix exists route.
    if (!fRouteIsValid) {
        cmd->setRoute(routeFromSenderToHere);
        cmd->completeRoute(false);

        // send bodcast if route is invalid
        if (!sendData(cmd, cmd->targetAddress(), &pkg.hdr)) {
            return ParserResult::Error;
        }
    }

    _router->addProcesedPackage(cmd->packageId());
    return ParserResult::Processed;

}

ParserResult NetworkNode::workWithNetworkRequest(AbstractData *networkRequest,
                                              const AbstractNodeInfo *sender) {
    // convert transoprt pakcage
    auto cmd = dynamic_cast<NetworkRequest*>(networkRequest);

    if (!cmd)
        return ParserResult::Error;

    if (cmd->isComplete()) {
        if (cmd->askedNodes().contains(nodeId())) {
            return parsePackage(cmd->dataResponce(), sender);
        }

        return ParserResult::Processed;
    }

    return parsePackage(cmd->dataRequest(), sender);
}

void NetworkNode::incomingData(AbstractData *pkg, const HostAddress &sender) {
    AbstractNode::incomingData(pkg, sender);
}

QString NetworkNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

AbstractNodeInfo *NetworkNode::createNodeInfo(QAbstractSocket *socket,
                                           const HostAddress* clientAddress) const {
    return  new BaseNodeInfo(socket, clientAddress);
}

SqlDBCache *NetworkNode::db() const {
    return _db;
}

// TO-DO
bool NetworkNode::workWithSubscribe(const WebSocket &rec,
                                 const BaseId &clientOrNodeid) {

    auto _db = db();
    if (_db)
        return false;

    switch (static_cast<WebSocketRequest>(rec.getRequestCmd())) {

    case WebSocketRequest::Subscribe: {
        return _webSocketWorker->subscribe(clientOrNodeid, rec.address());
    }

    case WebSocketRequest::Unsubscribe: {
        _webSocketWorker->unsubscribe(clientOrNodeid, rec.address());
        return true;
    }

    case WebSocketRequest::SubscribeList: {

        WebSocketSubscriptions resp;
        resp.setAddresses(_webSocketWorker->list(clientOrNodeid));

        return sendData(&resp, clientOrNodeid);
    }

    default: break;
    }

    return false;
}

QVariantMap NetworkNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + _localNodeName + "/" + _localNodeName + "_" + DEFAULT_DB_NAME},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

bool NetworkNode::sendData(AbstractData *resp,
                        const HostAddress &addere,
                        const Header *req) {
    return AbstractNode::sendData(resp, addere, req);
}

bool NetworkNode::sendData(const AbstractData *resp,
                        const HostAddress &addere,
                        const Header *req) {

    return AbstractNode::sendData(resp, addere, req);
}



bool NetworkNode::sendData(AbstractData *resp,
                        const BaseId &nodeId,
                        const Header *req) {


    if (!resp || !resp->prepareToSend()) {
        return false;
    }

    return sendData(const_cast<const AbstractData*>(resp), nodeId, req);
}

bool NetworkNode::sendData(const AbstractData *resp, const BaseId &nodeId, const Header *req) {
    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value());
        if (info && info->isKnowAddress(nodeId)) {
            return sendData(resp, it.key(), req);
        }
    }

    auto brodcast = [this, &nodes, req](const AbstractData *data){
        bool result = false;
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            result = result || sendData(data, it.key(), req);
        }

        return result;
    };


    if (resp->cmd() != H_16<TransportData>()) {
        TransportData data(address());
        data.setTargetAddress(nodeId);
        if (!data.setData(*resp)) {
            return false;
        }

        data.setRoute(_router->getRoute(nodeId));
        data.setSenderID(this->nodeId());
        data.prepareToSend();

        return brodcast(&data);
    }

    return brodcast(resp);
}

void NetworkNode::badRequest(const HostAddress &address, const Header &req, const QString msg) {
    AbstractNode::badRequest(address, req, msg);
}

void NetworkNode::badRequest(const BaseId& address, const Header &req, const QString msg) {

    if (!changeTrust(address, REQUEST_ERROR)) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because trust not changed",
                                    QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(msg);
    if (!sendData(&bad, address, &req)) {
        return;
    }

    QuasarAppUtils::Params::log("Bad request sendet to adderess: " +
                                address.toBase64(),
                                QuasarAppUtils::Info);
}

bool NetworkNode::changeTrust(const HostAddress &id, int diff) {
    return AbstractNode::changeTrust(id, diff);
}

bool NetworkNode::changeTrust(const BaseId &id, int diff) {
    if (!_db)
        return false;

    auto client = _db->getObject(NodeObject{id});

    if (!client) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because client == null",
                                    QuasarAppUtils::Error);
        return false;
    }

    auto clone = client->clone().staticCast<NodeObject>();
    clone->changeTrust(diff);

    if (!_db->saveObject(clone.data())) {
        return false;
    }

    // to do
    // send all network that node id is trusted changed

    return true;
}

bool NetworkNode::ping(const BaseId &id) {
    LongPing cmd(nodeId());
    return sendData(&cmd, id);
}

DBOperationResult NP::NetworkNode::getObject(const NP::BaseId &requester,
                                          const NP::DBObject &templateObj,
                                          const DBObject** result) const {

    if (!_db && !result) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = _db->checkPermision(requester, templateObj.dbAddress(), Permission::Read);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    auto obj = _db->getObject(templateObj);
    if (!obj || (obj->dbAddress() != templateObj.dbAddress())) {
        return DBOperationResult::Unknown;
    }

    *result = obj;
    return DBOperationResult::Allowed;
}

DBOperationResult NetworkNode::getObjects(const BaseId &requester,
                                       const DBObject &templateObj,
                                       QList<const DBObject *> *result) const {
    if (!_db && !result) {
        return DBOperationResult::Unknown;
    }

    if (!_db->getAllObjects(templateObj, *result)) {
        return DBOperationResult::Unknown;
    }

    for (const auto& obj: *result) {
        if (!obj)
            return DBOperationResult::Unknown;

        auto permisionResult = _db->checkPermision(requester, obj->dbAddress(), Permission::Read);
        if (permisionResult != DBOperationResult::Allowed) {
            return permisionResult;
        }
    }

    return DBOperationResult::Allowed;
}

DBOperationResult NetworkNode::setObject(const BaseId &requester,
                                      const DBObject *saveObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = _db->checkPermision(requester,
                                               saveObject->dbAddress(),
                                               Permission::Write);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    if (!_db->saveObject(saveObject)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

bool NetworkNode::savePermision(const NetworkMember &node,
                             const NodesPermisionObject &permision) {
    return false;
}

DBOperationResult NetworkNode::deleteObject(const BaseId &requester,
                                         const DBObject *dbObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = _db->checkPermision(requester, dbObject->dbAddress(),
                                               Permission::Write);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    if (!_db->deleteObject(dbObject)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

}

