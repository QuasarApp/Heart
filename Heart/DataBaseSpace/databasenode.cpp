/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "databasenode.h"
#include "abstractnodeinfo.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"
#include "asyncsqldbwriter.h"

#include <badrequest.h>
#include <quasarapp.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <websocketcontroller.h>
#include <QCoreApplication>
#include <qsecretrsa2048.h>
#include <ping.h>
#include <keystorage.h>
#include <basenodeinfo.h>
#include <networkmember.h>
#include <memberpermisionobject.h>
#include <permisioncontrolmember.h>

#define THIS_NODE "this_node_key"
namespace QH {
using namespace PKG;

DataBaseNode::DataBaseNode(QObject *ptr):
    AbstractNode(ptr) {

    _webSocketWorker = new WebSocketController(this);

}

bool DataBaseNode::initSqlDb(QString DBparamsFile,
                             SqlDBCache *cache,
                             SqlDBWriter *writer) {

    initDefaultDbObjects(cache, writer);

    QVariantMap params;
    _db->setSQLSources(SQLSources());

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


bool DataBaseNode::welcomeAddress(const HostAddress&) {
    return true;
}

bool DataBaseNode::isBanned(const BaseId &node) const {
    PermisionControlMember member(node);
    auto objectFromDataBase = db()->getObject<NetworkMember>(member);

    return objectFromDataBase->trust() <= 0;
}

QStringList DataBaseNode::SQLSources() const{
    return {
        DEFAULT_DB_INIT_FILE_PATH
    };
}

void DataBaseNode::nodeConnected(const HostAddress &node) {
    AbstractNode::nodeConnected(node);
    welcomeAddress(node);
}

QString DataBaseNode::dbLocation() const {
    if (db() && db()->writer()) {
        return db()->writer()->databaseLocation();
    }

    return "";
}


bool DataBaseNode::sendData(AbstractData *resp,
                        const HostAddress &addere,
                        const Header *req) {
    return AbstractNode::sendData(resp, addere, req);
}

bool DataBaseNode::sendData(const AbstractData *resp,
                        const HostAddress &addere,
                        const Header *req) {

    return AbstractNode::sendData(resp, addere, req);
}



bool DataBaseNode::sendData(AbstractData *resp,
                        const BaseId &nodeId,
                        const Header *req) {


    if (!resp || !resp->prepareToSend()) {
        return false;
    }

    return sendData(const_cast<const AbstractData*>(resp), nodeId, req);
}

AbstractNodeInfo *DataBaseNode::createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const {
    return new BaseNodeInfo(socket, clientAddress);;
}

bool DataBaseNode::sendData(const AbstractData *resp, const BaseId &nodeId, const Header *req) {
    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value());
        if (info && info->selfId() == nodeId) {
            return sendData(resp, it.key(), req);
        }
    }

    return false;
}

void DataBaseNode::badRequest(const HostAddress &address, const Header &req,
                              const ErrorData &err, quint8 diff) {
    AbstractNode::badRequest(address, req, err, diff);
}

void DataBaseNode::badRequest(const BaseId &address, const Header &req,
                              const ErrorData &err, quint8 diff) {

    if (!changeTrust(address, diff)) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because trust not changed",
                                    QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(err);
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

    auto client = _db->getObject<NetworkMember>(PermisionControlMember{id});

    if (!client) {
        return false;
    }

    auto clone = client->clone().staticCast<PermisionControlMember>();
    clone->changeTrust(diff);

    if (!_db->saveObject(clone.data())) {
        return false;
    }

    return true;
}

ParserResult DataBaseNode::parsePackage(const Package &pkg,
                                        const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<WebSocket>() == pkg.hdr.command) {
        WebSocket obj(pkg);

        BaseId requesterId = getSender(sender, &obj);

        if (!obj.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr,
                       {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        if (!workWithSubscribe(obj, requesterId, *sender)) {
            badRequest(sender->networkAddress(), pkg.hdr, {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg.hdr.command) {
        WebSocketSubscriptions obj(pkg);
        if (!obj.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr, {
                           ErrorCodes::InvalidRequest,
                           "WebSocketSubscriptions request is invalid"
                       });
            return ParserResult::Error;
        }

        incomingData(&obj, sender->networkAddress());
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;
}

QString DataBaseNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

SqlDBCache *DataBaseNode::db() const {
    return _db;
}

bool DataBaseNode::workWithSubscribe(const WebSocket &rec,
                                     const BaseId &clientOrNodeid,
                                     const AbstractNodeInfo & sender) {

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

        return sendData(&resp, sender.networkAddress());
    }

    default: break;
    }

    return false;
}

QVariantMap DataBaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + _localNodeName + "/" + _localNodeName + "_" + DEFAULT_DB_NAME},
    };
}

DBOperationResult QH::DataBaseNode::getObject(const QH::BaseId &requester,
                                              const QH::DBObject &templateObj,
                                              const DBObject** result) const {

    if (!_db && !result) {
        return DBOperationResult::Unknown;
    }

    DBOperationResult permisionResult = checkPermission(requester, templateObj.dbAddress(),
                                                        Permission::Read);
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

        auto permisionResult = checkPermission(requester, obj->dbAddress(),
                                               Permission::Read);
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

    auto permisionResult = checkPermission(requester,
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

BaseId DataBaseNode::getSender(const AbstractNodeInfo *connectInfo, const AbstractData *) const {

    auto info = dynamic_cast<const BaseNodeInfo*>(connectInfo);

    if (!info)
        return {};

    return info->selfId();
}

DBOperationResult DataBaseNode::checkPermission(const BaseId &requester,
                                                const DbAddress &objectAddress,
                                                const Permission& requarimentPermision) const {
     const NetworkMember *member = _db->getObject(PermisionControlMember{requester});
     if (!member) {
         return DBOperationResult::Unknown;
     }

     const MemberPermisionObject *permision =
             _db->getObject(MemberPermisionObject({requester, objectAddress}));

     if (!permision) {
         return DBOperationResult::Unknown;
     }

     if (permision->permisions() < requarimentPermision) {
         return DBOperationResult::Forbidden;
     }

     return DBOperationResult::Allowed;
}

DBOperationResult DataBaseNode::deleteObject(const BaseId &requester,
                                             const DBObject *dbObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = checkPermission(requester,
                                           dbObject->dbAddress(),
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

