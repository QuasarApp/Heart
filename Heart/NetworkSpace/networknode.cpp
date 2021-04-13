/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include <abstractnetworkmember.h>
#include <networknodeinfo.h>
#include <nodeobject.h>
#include <networkmember.h>
#include <QSharedPointer>
#include "networkerrorcodes.h"



#define THIS_NODE "this_node_key"
namespace QH {

using namespace PKG;

NetworkNode::NetworkNode(QObject *ptr):
    DataBaseNode(ptr) {

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

NodeId NetworkNode::nodeId() const {

    auto keys = _nodeKeys->getNextPair(THIS_NODE);
    return QCryptographicHash::hash(keys.publicKey(), QCryptographicHash::Sha256);
}

unsigned int NetworkNode::sendData(AbstractData *resp, const QVariant &nodeId, const Header *req) {
    return sendData(resp, NodeId(nodeId), req);
}

unsigned int NetworkNode::sendData(const AbstractData *resp, const QVariant  &nodeId, const Header *req) {
    return sendData(resp, NodeId(nodeId), req);
}

unsigned int NetworkNode::sendData(AbstractData *resp,
                           const HostAddress &nodeId,
                           const Header *req) {
    return DataBaseNode::sendData(resp, nodeId, req);
}

unsigned int NetworkNode::sendData(const AbstractData *resp,
                           const HostAddress &nodeId,
                           const Header *req) {
    return DataBaseNode::sendData(resp, nodeId, req);
}

unsigned int NetworkNode::sendData(const AbstractData *resp,
                           const NodeId &nodeId,
                           const Header *req) {
    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<NetworkNodeInfo*>(it.value());
        if (info && info->isKnowAddress(nodeId)) {
            return sendData(resp, it.key(), req);
        }
    }

    auto brodcast = [this, &nodes, req](const AbstractData *data) -> unsigned int {
        bool result = false;
        unsigned int hashCode = 0;
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            hashCode = sendData(data, it.key(), req);
            result += hashCode;
        }

        if (result) {
            return hashCode;
        }

        return 0;
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

unsigned int NetworkNode::sendData(AbstractData *resp, const NodeId &nodeId, const Header *req) {
    if (!resp || !resp->prepareToSend()) {
        return false;
    }

    return sendData(const_cast<const AbstractData*>(resp), nodeId, req);
}


bool NetworkNode::checkSignOfRequest(const AbstractData *request) {
    auto object = dynamic_cast<const Sign*>(request);
    auto dbObject = dynamic_cast<const SenderData*>(request);

    if (!(object && dbObject)) {
        return false;
    }

    auto node = db()->getObject(NetworkMember{dbObject->senderID().toRaw()});
    return _nodeKeys->check(_nodeKeys->concatSign(object->dataForSigned(),
                                                  object->sign()), node->authenticationData());
}

void NetworkNode::thisNode(NodeObject& res) const {
    auto keys = _nodeKeys->getNextPair(THIS_NODE);

    res.setAuthenticationData(keys.publicKey());
    res.setTrust(0);

    res.prepareToSend();

}

QSet<NodeId> NetworkNode::myKnowAddresses() const {
    QSet<NodeId> res;
    const auto connects = connections();
    for (const AbstractNodeInfo *i : connects) {
        auto info = dynamic_cast<const NetworkNodeInfo*>(i);
        if (info && info->selfId().isValid()) {
            res += info->selfId();
        }
    }

    return res;
}

bool NetworkNode::welcomeAddress(AbstractNodeInfo *node) {
    NodeObject self;
    thisNode(self);

    if (!sendData(&self, node->networkAddress())) {
        return false;
    }

    if (connections().size()) {

        auto knowAddresses = myKnowAddresses();
        if (knowAddresses.size()) {
            KnowAddresses addressesData;
            addressesData.setKnowAddresses(knowAddresses);

            if (!sendData(&addressesData, node->networkAddress())) {
                return false;
            }
        }
    }

    return true;

}

void NetworkNode::nodeConnected(AbstractNodeInfo *node) {
    DataBaseNode::nodeConnected(node);
}

void NetworkNode::nodeConfirmend(AbstractNodeInfo *node) {
    DataBaseNode::nodeConfirmend(node);

    auto nodeInfo = dynamic_cast<NetworkNodeInfo*>(node);
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    NetworkNodeInfo* oldNodeInfo = _connections.value(nodeInfo->selfId(), nullptr);
    _connectionsMutex.unlock();

    if (oldNodeInfo) {
        removeNode(node->networkAddress());
        return;
    }

    _connectionsMutex.lock();
    _connections.insert(nodeInfo->selfId(), nodeInfo);

    _connectionsMutex.unlock();

}

void NetworkNode::nodeDisconnected(AbstractNodeInfo *node) {
    AbstractNode::nodeDisconnected(node);

    auto nodeInfo = dynamic_cast<NetworkNodeInfo*>(node);
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    _connections.remove(nodeInfo->selfId());
    _connectionsMutex.unlock();
}

void NetworkNode::incomingData(AbstractData *, const NodeId &) {}

void NetworkNode::incomingData(AbstractData *pkg, const AbstractNodeInfo *sender) {
    DataBaseNode::incomingData(pkg, sender);
}

QString NetworkNode::keyStorageLocation() const {
    return _nodeKeys->storageLocation();
}

ParserResult NetworkNode::parsePackage(const QSharedPointer<AbstractData> &pkg,
                                       const Header &pkgHeader,
                                       const AbstractNodeInfo *sender) {
    auto parentResult = DataBaseNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto baseSender = dynamic_cast<const NetworkNodeInfo*>(sender);
    if (!baseSender) {
        QuasarAppUtils::Params::log("Sender is not basenode info!",
                                    QuasarAppUtils::Error);
        return ParserResult::Error;
    }

    if (H_16<BadNodeRequest>() == pkgHeader.command) {
        auto cmd = pkg.staticCast<BadNodeRequest>();

        incomingData(cmd.data(), baseSender->selfId());
        emit requestError(cmd->errCode(), cmd->err());

        return ParserResult::Processed;

    } else if (H_16<TransportData>() == pkgHeader.command) {
        auto cmd = pkg.staticCast<TransportData>();

        return workWithTransportData(cmd.data(), sender, pkgHeader);


    } else if (H_16<NodeObject>() == pkgHeader.command) {
        auto obj = QSharedPointer<NodeObject>::create(pkg);
        if (!obj->isValid()) {
            badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "NodeObject request is invalid"
                       }
                       );
            return ParserResult::Error;
        }

        if (!workWithNodeObjectData(obj, sender)) {
            badRequest(obj->senderID(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "NodeObject request is invalid"
                       }
                       );
            return ParserResult::Error;
        }

        return ParserResult::Processed;
    } else if (H_16<KnowAddresses>() == pkgHeader.command) {
        auto obj = pkg.staticCast<KnowAddresses>();

        if (!obj->isValid()) {
            badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "KnowAddresses request is invalid"
                       }
                       );
            return ParserResult::Error;
        }

        if (!workWithKnowAddresses(*obj, sender)) {
            badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "KnowAddresses request is invalid"
                       }
                       );
            return ParserResult::Error;
        }

        return ParserResult::Processed;
    } else if (H_16<LongPing>() == pkgHeader.command) {
        auto cmd = pkg.staticCast<LongPing>();

        if (!cmd->ansver()) {
            cmd->setAnsver(true);
            sendData(cmd.data(), cmd->senderID(), &pkgHeader);
        }

        incomingData(cmd.data(), baseSender->selfId());
        return ParserResult::Processed;
    } else if (H_16<NetworkRequest>() == pkgHeader.command) {
        auto cmd = pkg.staticCast<NetworkRequest>();

        if (!cmd->isValid()) {
            badRequest(baseSender->selfId(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "NetworkRequest request is invalid"
                       }
                       );
            return ParserResult::Error;
        }

        workWithNetworkRequest(cmd.data(), baseSender);
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;
}

bool NetworkNode::workWithNodeObjectData(const QSharedPointer<NodeObject> &node,
                                      const AbstractNodeInfo* nodeInfo) {

    if (!db()) {
        return false;
    }

    auto localObjec = db()->getObject(*node.data());

    if (localObjec) {
        node->setTrust(std::min(node->trust(), localObjec->trust()));

        if (!db()->updateObject(node)) {
            QuasarAppUtils::Params::log("Error on precessing of NodeObject updateObject is failed");
            return false;
        };
    } else {
        node->setTrust(0);

        if (!db()->insertObject(node)) {
            QuasarAppUtils::Params::log("Error on precessing of NodeObject insertObject is failed");
            return false;
        };
    }

    auto peerNodeInfo = dynamic_cast<NetworkNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo) {
        QuasarAppUtils::Params::log("Error on precessing of NodeObject peerNodeInfo is null");
        return false;
    }

    peerNodeInfo->setSelfId(NodeId(node->getId()));

    return true;
}

bool NetworkNode::workWithKnowAddresses(const KnowAddresses &obj,
                                        const AbstractNodeInfo *nodeInfo) {

    auto peerNodeInfo = dynamic_cast<NetworkNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->addKnowAddresses(obj.knowAddresses());

    return true;
}

ParserResult NetworkNode::workWithTransportData(const QSharedPointer<PKG::AbstractData> & transportData,
                                             const AbstractNodeInfo* sender,
                                             const Header& hdr) {

    // convert transoprt pakcage
    auto cmd = transportData.dynamicCast<TransportData>();

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

        return parsePackage(transportData, hdr, sender);
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
        while (it != cmd->route().rend() && !sendData(cmd.data(), *it, &hdr)) {
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    routeFromSenderToHere.erase(routeFromSenderToHere.cbegin(), routeFromSenderToHere.cbegin() + index);
#else
    routeFromSenderToHere.erase(routeFromSenderToHere.begin(), routeFromSenderToHere.begin() + index);
#endif

    // inversion route and update route to sender
    _router->updateRoute(cmd->senderID(),
    {routeFromSenderToHere.rbegin(), routeFromSenderToHere.rend()});

    // remove invalid route nodes and fix exists route.
    if (!fRouteIsValid) {
        cmd->setRoute(routeFromSenderToHere);
        cmd->completeRoute(false);

        // send bodcast if route is invalid
        if (!sendData(cmd.data(), cmd->targetAddress(), &hdr)) {
            return ParserResult::Error;
        }
    }

    _router->addProcesedPackage(cmd->packageId());
    return ParserResult::Processed;

}

ParserResult NetworkNode::workWithNetworkRequest(const QSharedPointer<PKG::AbstractData> &networkRequest,
                                              const AbstractNodeInfo *sender) {
    // convert transoprt pakcage
    auto cmd = networkRequest.dynamicCast<NetworkRequest>();

    if (!cmd)
        return ParserResult::Error;

    if (cmd->isComplete()) {
        if (cmd->askedNodes().contains(nodeId())) {
            auto data = prepareData(cmd->dataResponce());
            if (!data)
                return ParserResult::Error;

            return parsePackage(data, sender);
        }

        return ParserResult::Processed;
    }

    return parsePackage(cmd->dataRequest(), sender);
}

void NetworkNode::incomingData(AbstractData *pkg, const HostAddress &sender) {
    AbstractNode::incomingData(pkg, sender);
}

AbstractNodeInfo *NetworkNode::createNodeInfo(QAbstractSocket *socket,
                                           const HostAddress* clientAddress) const {
    return  new NetworkNodeInfo(socket, clientAddress);
}

void NetworkNode::badRequest(const HostAddress &address, const Header &req,
                             const ErrorData &err, qint8 diff) {
    DataBaseNode::badRequest(address, req, err, diff);
}

void NetworkNode::badRequest(const NodeId &address, const Header &req,
                             const ErrorData &err, qint8 diff) {
    if (!changeTrust(address, diff)) {

        QuasarAppUtils::Params::log("Bad request detected, bud response command not sent!"
                                    " because trust not changed",
                                    QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(err);
    if (!sendData(&bad, address, &req)) {
        return;
    }

    QuasarAppUtils::Params::log("Bad request sendet to adderess: " +
                                address.toString(),
                                QuasarAppUtils::Info);
}

bool NetworkNode::changeTrust(const QVariant &id, int diff) {
    return DataBaseNode::changeTrust(id, diff);
}

bool NetworkNode::changeTrust(const HostAddress &id, int diff) {
    return DataBaseNode::changeTrust(id, diff);
}

QStringList NetworkNode::SQLSources() const {
    return {":/sql/NetworkSpace/Res/NetworkDB.sql"};
}

bool NetworkNode::changeTrust(const NodeId &id, int diff) {
    return DataBaseNode::changeTrust(id.toRaw(), diff);
}

bool NetworkNode::ping(const NodeId &id) {
    LongPing cmd(nodeId());
    return sendData(&cmd, id);
}

}

