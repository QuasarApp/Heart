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
#include <nodeobject.h>


#define THIS_NODE "this_node_key"
namespace QH {

NetworkNode::NetworkNode(QH::SslMode mode, QObject *ptr):
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

NodeObject NetworkNode::thisNode() const {
    NodeObject res;
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
    NodeObject self = thisNode();

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


    } else if (H_16<NodeObject>() == pkg.hdr.command) {
        NodeObject obj(pkg);
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

bool NetworkNode::workWithNodeObjectData(NodeObject &node,
                                      const AbstractNodeInfo* nodeInfo) {

    if (!db()) {
        return false;
    }

    auto localObjec = db()->getObject(node);

    if (localObjec) {
        node.setTrust(std::min(node.trust(), localObjec->trust()));
    } else {
        node.setTrust(0);
    }

    if (!db()->saveObject(&node)) {
        return false;
    };

    auto peerNodeInfo = dynamic_cast<NetworkNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->setSelfId(node.getId());

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

AbstractNodeInfo *NetworkNode::createNodeInfo(QAbstractSocket *socket,
                                           const HostAddress* clientAddress) const {
    return  new NetworkNodeInfo(socket, clientAddress);
}

bool NetworkNode::sendData(const AbstractData *resp, const BaseId &nodeId, const Header *req) {
    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<NetworkNodeInfo*>(it.value());
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

bool NetworkNode::sendData(AbstractData *resp, const BaseId &nodeId, const Header *req) {
    return DataBaseNode::sendData(resp, nodeId, req);
}

bool NetworkNode::sendData(const AbstractData *resp, const HostAddress &nodeId, const Header *req) {
    return DataBaseNode::sendData(resp, nodeId, req);
}

bool NetworkNode::sendData(AbstractData *resp, const HostAddress &nodeId, const Header *req) {
    return DataBaseNode::sendData(resp, nodeId, req);
}

bool NetworkNode::ping(const BaseId &id) {
    LongPing cmd(nodeId());
    return sendData(&cmd, id);
}

}

