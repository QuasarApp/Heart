/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "databasenode.h"
#include "basenodeinfo.h"
#include "deleteobjectrequest.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"
#include "nodeobject.h"
#include "nodespermisionobject.h"
#include "sign.h"
#include "asyncsqldbwriter.h"

#include <badrequest.h>
#include <quasarapp.h>
#include <availabledatarequest.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <websocketcontroller.h>
#include <QCoreApplication>
#include <qsecretrsa2048.h>
#include <ping.h>
#include <keystorage.h>

#define THIS_NODE "this_node_key"
namespace NP {

DataBaseNode::DataBaseNode(NP::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

    _webSocketWorker = new WebSocketController(this);
    _nodeKeys = new KeyStorage(new QSecretRSA2048());

}

bool DataBaseNode::initSqlDb(QString DBparamsFile,
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

bool DataBaseNode::isSqlInited() const {
    return _db;
}

bool DataBaseNode::run(const QString &addres, unsigned short port) {
    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }

    _nodeKeys->initDefaultStorageLocation();
    _nodeKeys->start();

    return AbstractNode::run(addres, port);
}

bool DataBaseNode::run(const QString &addres,
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

void DataBaseNode::stop() {

    AbstractNode::stop();

    if (db()) {
        auto writer = _db->writer();
        delete _db;
        _db = nullptr;
        delete writer;

    }

}

DataBaseNode::~DataBaseNode() {
    delete _nodeKeys;
}

void DataBaseNode::initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer) {
    if (!writer) {
        writer = new AsyncSqlDbWriter();
    }

    if (!cache) {
        cache = new SqlDBCache();
    }

    cache->setWriter(writer);
    _db = cache;

    connect(_db, &SqlDBCache::sigItemChanged,
            _webSocketWorker, &WebSocketController::handleItemChanged);
}

BaseId DataBaseNode::nodeId() const {

    auto keys = _nodeKeys->getNextPair(THIS_NODE);
    return QCryptographicHash::hash(keys.publicKey(), QCryptographicHash::Sha256);
}

bool DataBaseNode::connectToHost(const HostAddress &ip, SslMode mode) {
    if (!AbstractNode::connectToHost(ip, mode))
        return false;

    return true;
}

bool DataBaseNode::checkSignOfRequest(const AbstractData *request) {
    auto object = dynamic_cast<const Sign*>(request);
    auto dbObject = dynamic_cast<const SenderData*>(request);

    if (!(object && dbObject)) {
        return false;
    }

    auto node = _db->getObject(NodeObject{dbObject->senderID()});
    return _nodeKeys->check(_nodeKeys->concatSign(object->dataForSigned(),
                                                  object->sign()), node->publickKey());
}

NodeObject DataBaseNode::thisNode() const {
    NodeObject res;
    auto keys = _nodeKeys->getNextPair(THIS_NODE);

    res.setPublickKey(keys.publicKey());
    res.setTrust(0);

    res.prepareToSend();

    return res;
}

QSet<BaseId> DataBaseNode::myKnowAddresses() const {
    QSet<BaseId> res;

    for (const AbstractNodeInfo *i : connections()) {
        auto info = dynamic_cast<const BaseNodeInfo*>(i);
        if (info && info->selfId().isValid()) {
            res += info->selfId();
        }
    }

    return res;
}

void DataBaseNode::nodeConnected(const HostAddress &node) {
    AbstractNode::nodeConnected(node);
}

void DataBaseNode::nodeConfirmend(const HostAddress &node) {
    AbstractNode::nodeConfirmend(node);

    auto nodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(node));
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    BaseNodeInfo* oldNodeInfo = _connections.value(nodeInfo->selfId(), nullptr);
    _connectionsMutex.unlock();

    if (oldNodeInfo) {
        removeNode(node);
        return;
    }

    _connectionsMutex.lock();
    _connections.insert(nodeInfo->selfId(), nodeInfo);

    _connectionsMutex.unlock();

}

void DataBaseNode::nodeDisconnected(const HostAddress &node) {
    AbstractNode::nodeDisconnected(node);

    auto nodeInfo = dynamic_cast<BaseNodeInfo*>(getInfoPtr(node));
    if (!nodeInfo) {
        return;
    }

    _connectionsMutex.lock();
    _connections.remove(nodeInfo->selfId());
    _connectionsMutex.unlock();
}

void DataBaseNode::incomingData(AbstractData *, const BaseId &) {}

QString DataBaseNode::keyStorageLocation() const {
    return _nodeKeys->storageLocation();
}

QString DataBaseNode::dbLocation() const {
    if (db() && db()->writer()) {
        return db()->writer()->databaseLocation();
    }

    return "";
}

bool DataBaseNode::isBanned(const BaseId &node) const {
    NodeObject nodeObj(node);
    auto objectFromDataBase = db()->getObject(nodeObj);

    return objectFromDataBase->trust() <= 0;
}

ParserResult DataBaseNode::parsePackage(const Package &pkg,
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

    if (H_16<AvailableDataRequest>() == pkg.hdr.command) {
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

        incomingData(&obj, baseSender->selfId());
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

        incomingData(&obj, baseSender->selfId());
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
    }

    return ParserResult::NotProcessed;
}

bool DataBaseNode::workWithAvailableDataRequest(const AvailableDataRequest &rec,
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

bool DataBaseNode::workWithNodeObjectData(NodeObject& node,
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

void DataBaseNode::incomingData(AbstractData *pkg, const HostAddress &sender) {
    AbstractNode::incomingData(pkg, sender);
}

QString DataBaseNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

AbstractNodeInfo *DataBaseNode::createNodeInfo(QAbstractSocket *socket,
                                           const HostAddress* clientAddress) const {
    return  new BaseNodeInfo(socket, clientAddress);
}

SqlDBCache *DataBaseNode::db() const {
    return _db;
}

// TO-DO
bool DataBaseNode::workWithSubscribe(const WebSocket &rec,
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

QVariantMap DataBaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + _localNodeName + "/" + _localNodeName + "_" + DEFAULT_DB_NAME},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}



bool DataBaseNode::sendData(AbstractData *resp,
                        const BaseId &nodeId,
                        const Header *req) {


    if (!resp || !resp->prepareToSend()) {
        return false;
    }

    return sendData(const_cast<const AbstractData*>(resp), nodeId, req);
}

bool DataBaseNode::sendData(const AbstractData *resp, const BaseId &nodeId, const Header *req) {
    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value());
        if (info && info->isKnowAddress(nodeId)) {
            return sendData(resp, it.key(), req);
        }
    }

    return false;
}

void DataBaseNode::badRequest(const HostAddress &address, const Header &req, const QString msg) {
    AbstractNode::badRequest(address, req, msg);
}

void DataBaseNode::badRequest(const BaseId& address, const Header &req, const QString msg) {

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

bool DataBaseNode::changeTrust(const HostAddress &id, int diff) {
    return AbstractNode::changeTrust(id, diff);
}

bool DataBaseNode::changeTrust(const BaseId &id, int diff) {
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

DBOperationResult NP::DataBaseNode::getObject(const NP::BaseId &requester,
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

DBOperationResult DataBaseNode::getObjects(const BaseId &requester,
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

DBOperationResult DataBaseNode::setObject(const BaseId &requester,
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

DBOperationResult DataBaseNode::deleteObject(const BaseId &requester,
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

