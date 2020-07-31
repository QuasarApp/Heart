/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "basenode.h"
#include "basenodeinfo.h"
#include "deleteobjectrequest.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"
#include "nodeobject.h"
#include "nodespermisionobject.h"
#include "badnoderequest.h"
#include "nodeid.h"
#include "sign.h"

#include <badrequest.h>
#include <quasarapp.h>
#include <transportdata.h>
#include <availabledatarequest.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <websocketcontroller.h>
#include <QCoreApplication>
#include <qsecretrsa2048.h>
#include <ping.h>
#include <keystorage.h>
#include <knowaddresses.h>

#define THIS_NODE "this_node_key"
namespace NP {

BaseNode::BaseNode(NP::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

    _webSocketWorker = new WebSocketController(this);
    _nodeKeys = new KeyStorage(new QSecretRSA2048());

}

bool BaseNode::initSqlDb(QString DBparamsFile,
                         SqlDBCache *cache,
                         SqlDBWriter *writer) {

    initDefaultDbObjects(cache, writer);

    QVariantMap params;

    if (DBparamsFile.isEmpty()) {
        params = defaultDbParams();
        return _db->init(params);
    }

    if (!_db->init(DBparamsFile)) {
        return false;
    }

    return true;
}

bool BaseNode::isSqlInited() const {
    return _db;
}

bool BaseNode::run(const QString &addres, unsigned short port) {
    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }

    _nodeKeys->initDefaultStorageLocation();
    _nodeKeys->start();

    return AbstractNode::run(addres, port);
}

bool BaseNode::run(const QString &addres,
                   unsigned short port,
                   const QString &localNodeName) {

    if (localNodeName.isEmpty())
        return false;

    _localNodeName = localNodeName;

    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }

    _nodeKeys->initDefaultStorageLocation(localNodeName);
    _nodeKeys->start();

    return AbstractNode::run(addres, port);
}

BaseNode::~BaseNode() {
    delete _nodeKeys;
}

void BaseNode::initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer) {
    if (!writer) {
        writer = new SqlDBWriter();
    }

    if (!cache) {
        cache = new SqlDBCache();
    }

    cache->setWriter(writer);
    _db = cache;

    connect(_db, &SqlDBCache::sigItemChanged,
            _webSocketWorker, &WebSocketController::handleItemChanged);
}

BaseId BaseNode::nodeId() const {

    auto keys = _nodeKeys->getNextPair(THIS_NODE);
    return NodeId(QCryptographicHash::hash(keys.publicKey(), QCryptographicHash::Sha256));
}

bool BaseNode::connectToHost(const QHostAddress &ip, unsigned short port, SslMode mode) {
    if (!AbstractNode::connectToHost(ip, port, mode))
        return false;

    if (!welcomeAddress(ip)) {
        return false;
    }

    return true;
}

bool BaseNode::checkSignOfRequest(const AbstractData *request) {
    auto object = dynamic_cast<const Sign*>(request);
    auto dbObject = dynamic_cast<const SenderData*>(request);

    if (!(object && dbObject)) {
        return false;
    }

    auto node = _db->getObject(NodeObject{dbObject->senderID()});
    return _nodeKeys->check(_nodeKeys->concatSign(object->dataForSigned(),
                                                  object->sign()), node->publickKey());
}

NodeObject BaseNode::thisNode() const {
    NodeObject res;
    auto keys = _nodeKeys->getNextPair(THIS_NODE);

    res.setPublickKey(keys.publicKey());
    res.setTrust(0);

    return res;
}

QSet<BaseId> BaseNode::myKnowAddresses() const {
    QSet<BaseId> res;

    for (const NodeInfoData &i : connections()) {
        auto info = dynamic_cast<const BaseNodeInfo*>(i.info);
        if (info && info->selfId().isValid()) {
            res += info->selfId();
        }
    }

    return res;
}

bool BaseNode::welcomeAddress(const QHostAddress& ip) const {
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

void BaseNode::connectionRegistered(const AbstractNodeInfo *info) {
    welcomeAddress(info->networkAddress());
}

ParserResult BaseNode::parsePackage(const Package &pkg,
                                    const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<BadNodeRequest>() == pkg.hdr.command) {
        BadNodeRequest cmd(pkg);

        incomingData(&cmd, sender->networkAddress());
        emit requestError(cmd.err());

        return ParserResult::Processed;

    } else if (H_16<TransportData>() == pkg.hdr.command) {
        TransportData cmd(pkg);

        if (cmd.targetAddress() == nodeId()) {
            return parsePackage(cmd.data(), sender);
        }

        if (!sendData(&cmd, cmd.targetAddress(), &pkg.hdr)) {
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<AvailableDataRequest>() == pkg.hdr.command) {
        AvailableDataRequest cmd(pkg);

        if (!cmd.isValid()) {
            badRequest(cmd.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithAvailableDataRequest(cmd, &pkg.hdr)) {
            badRequest(cmd.senderID(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<AvailableData>() == pkg.hdr.command) {
        AvailableData obj(pkg);
        if (!obj.isValid()) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        incomingData(&obj, sender->networkAddress());
        return ParserResult::Processed;

    } else if (H_16<DeleteObjectRequest>() == pkg.hdr.command) {
        DeleteObjectRequest obj(pkg);

        if (!checkSignOfRequest(&obj)) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        DBOperationResult result = deleteObject(obj.senderID(), &obj);

        switch (result) {
        case DBOperationResult::Forbidden:
            return ParserResult::Error;

        case DBOperationResult::Unknown: {
            // TO DO Work with request for netwotk.
            return ParserResult::Processed;
        }
        case DBOperationResult::Allowed: {
            return ParserResult::Processed;
        }
        }


    } else if (H_16<WebSocket>() == pkg.hdr.command) {
        WebSocket obj(pkg);

        if (!checkSignOfRequest(&obj)) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!obj.isValid()) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithSubscribe(obj, obj.senderID())) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg.hdr.command) {
        WebSocketSubscriptions obj(pkg);
        if (!obj.isValid()) {
            badRequest(obj.senderID(), pkg.hdr);
            return ParserResult::Error;
        }

        incomingData(&obj, sender->networkAddress());
        return ParserResult::Processed;
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
    } else if (H_16<NodeObject>() == pkg.hdr.command) {
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
    }
    return ParserResult::NotProcessed;
}

bool BaseNode::workWithAvailableDataRequest(const AvailableDataRequest &rec,
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

bool BaseNode::workWithNodeObjectData(NodeObject& node,
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

    if (DBOperationResult::Allowed == setObject(nodeId(), &node)) {
        return false;
    };

    auto peerNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->setSelfId(node.nodeId());

    return true;
}

bool BaseNode::workWithKnowAddresses(const KnowAddresses &obj,
                                     const AbstractNodeInfo *nodeInfo) {

    auto peerNodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(nodeInfo->networkAddress()));
    if (!peerNodeInfo)
        return false;

    peerNodeInfo->addKnowAddresses(obj.knowAddresses());

    return true;
}

ParserResult BaseNode::workWithTransportData(AbstractData *transportData,
                                             const AbstractNodeInfo* sender,
                                             const Package& pkg) {
    auto cmd = dynamic_cast<TransportData*>(transportData);

    if (!cmd)
        return ParserResult::Error;

    if (cmd->targetAddress() == nodeId()) {
        return parsePackage(cmd->data(), sender);
    }

    // dead end. i am need to work with optimisation data of route in the another threade.
    auto sendValue = [this, &cmd, &pkg](){

        if (!sendData(cmd, cmd->targetAddress(), &pkg.hdr)) {
            return false;
        }

        return true;
    };

    if (cmd->isRouteComplete()) {

        int index = cmd->route().indexOf(address());

        optimisationRoute(cmd->route()) ;
    }

    cmd->addNodeToRoute(address());

    if (!sendData(cmd, cmd->targetAddress(), &pkg.hdr)) {
        return ParserResult::Error;
    }

}

QString BaseNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

AbstractNodeInfo *BaseNode::createNodeInfo(QAbstractSocket *socket) const {
    return  new BaseNodeInfo(socket);
}

SqlDBCache *BaseNode::db() const {
    return _db;
}

// TO-DO
bool BaseNode::workWithSubscribe(const WebSocket &rec,
                                 const BaseId &clientOrNodeid) {

    auto _db = db();
    if (_db)
        return false;

    switch (static_cast<WebSocketRequest>(rec.getRequestCmd())) {

    case WebSocketRequest::Subscribe: {
        return _webSocketWorker->subscribe(clientOrNodeid, rec.dbAddress());
    }

    case WebSocketRequest::Unsubscribe: {
        _webSocketWorker->unsubscribe(clientOrNodeid, rec.dbAddress());
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

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + _localNodeName + "/" + DEFAULT_DB_NAME},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

bool BaseNode::sendData(const AbstractData *resp,
                        const QHostAddress &addere,
                        const Header *req) const {

    return AbstractNode::sendData(resp, addere, req);
}

bool BaseNode::sendData(const AbstractData *resp,
                        const BaseId &nodeId,
                        const Header *req) const {

    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value().info);
        if (info && info->isKnowAddress(nodeId)) {
            return sendData(resp, it.key(), req);
        }
    }

    TransportData data;
    data.setTargetAddress(nodeId);
    data.setData(*resp);
    bool result = false;
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        result = result || sendData(&data, it.key(), req);
    }

    return result;
}

void BaseNode::badRequest(const QHostAddress &address, const Header &req, const QString msg) {
    AbstractNode::badRequest(address, req, msg);
}

void BaseNode::badRequest(const BaseId& address, const Header &req, const QString msg) {

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

bool BaseNode::changeTrust(const QHostAddress &id, int diff) {
    return AbstractNode::changeTrust(id, diff);
}

bool BaseNode::changeTrust(const BaseId &id, int diff) {
    if (!_db)
        return false;

    auto client = _db->getObject(NodeObject{id});

    if (!client) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because client == null",
                                    QuasarAppUtils::Error);
        return false;
    }

    client->changeTrust(diff);

    if (!_db->saveObject(client)) {
        return false;
    }

    // to do
    // send all network that node id is trusted changed

    return true;
}

bool BaseNode::ping(const BaseId &id) {
    Ping cmd;
    return sendData(&cmd, id);
}

DBOperationResult NP::BaseNode::getObject(const NP::BaseId &requester,
                                          const NP::DBObject &templateObj,
                                          DBObject** result) const {

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

DBOperationResult BaseNode::getObjects(const BaseId &requester,
                                       const DBObject &templateObj,
                                       QList<DBObject *> *result) const {
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

DBOperationResult BaseNode::setObject(const BaseId &requester,
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

DBOperationResult BaseNode::deleteObject(const BaseId &requester,
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

