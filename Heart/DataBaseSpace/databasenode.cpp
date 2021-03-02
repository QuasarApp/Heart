/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include <ping.h>
#include <keystorage.h>
#include <basenodeinfo.h>
#include <abstractnetworkmember.h>
#include <memberpermisionobject.h>
#include <networkmember.h>
#include <deleteobject.h>
#include "dberrorcodes.h"
#include <QSet>
#include <isubscribabledata.h>
#include <itoken.h>
#include <sqlitedbcache.h>

#define THIS_NODE "this_node_key"
namespace QH {
using namespace PKG;

DataBaseNode::DataBaseNode(QObject *ptr):
    AbstractNode(ptr) {

    _webSocketWorker = new WebSocketController(this);

    qRegisterMetaType<QSharedPointer<QH::PKG::DBObject>>();

    registerPackageType<WebSocket>();
    registerPackageType<WebSocketSubscriptions>();
    registerPackageType<DeleteObject>();

}

bool DataBaseNode::initSqlDb(QString DBparamsFile,
                             ISqlDBCache *cache,
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
        _db->softDelete();
        _db = nullptr;
        delete writer;

    }

}

DataBaseNode::~DataBaseNode() {
}

void DataBaseNode::initDefaultDbObjects(ISqlDBCache *cache,
                                        SqlDBWriter *writer) {
    if (!writer) {
        writer = new AsyncSqlDbWriter();
    }

    if (!cache) {
        cache = new SQLiteDBCache();
    }

    cache->setWriter(writer);
    _db = cache;

    connect(_db, &SqlDBCache::sigItemChanged,
            this, &DataBaseNode::hendleObjectChanged);
}


bool DataBaseNode::welcomeAddress(AbstractNodeInfo *) {
    return true;
}

bool DataBaseNode::isBanned(const QVariant &node) const {
    NetworkMember member(node);
    auto objectFromDataBase = db()->getObject<AbstractNetworkMember>(member);

    return objectFromDataBase->trust() <= 0;
}

QStringList DataBaseNode::SQLSources() const{
    return {
        DEFAULT_DB_INIT_FILE_PATH
    };
}

QSet<QString> DataBaseNode::systemTables() const {
    return {"NetworkMembers", "MemberPermisions"};
}

bool DataBaseNode::objectChanged(const QSharedPointer<PKG::ISubscribableData> &item) {

    if (!item.dynamicCast<PKG::AbstractData>()) {
        return false;
    }

    _webSocketWorker->handleItemChanged(item);

    return true;
}

void DataBaseNode::hendleObjectChanged(const QSharedPointer<DBObject> &item) {
    objectChanged(item.staticCast<PKG::ISubscribableData>());
}

void DataBaseNode::nodeConnected(AbstractNodeInfo *node) {
    AbstractNode::nodeConnected(node);
    welcomeAddress(node);
}

void DataBaseNode::memberSubsribed(const QVariant &, unsigned int ) {
    return;
}

void DataBaseNode::memberUnsubsribed(const QVariant &, unsigned int ) {
    return;
}

QString DataBaseNode::dbLocation() const {
    if (db() && db()->writer()) {
        return db()->writer()->databaseLocation();
    }

    return "";
}

AbstractNodeInfo *DataBaseNode::createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const {
    return new BaseNodeInfo(socket, clientAddress);
}

bool DataBaseNode::changeTrust(const HostAddress &id, int diff) {    
    auto info = dynamic_cast<const BaseNodeInfo*>(getInfoPtr(id));
    if (!info)
        return false;

    if (info->id().isValid())
        return changeTrust(info->id(), diff);

    return AbstractNode::changeTrust(id, diff);
}

bool DataBaseNode::changeTrust(const QVariant &id, int diff) {
    if (!_db)
        return false;

    auto action = [diff](const QSharedPointer<DBObject> &object) {
        auto obj = object.dynamicCast<AbstractNetworkMember>();
        if (!obj) {
            return false;
        }

        obj->changeTrust(diff);

        return true;
    };

    return _db->changeObjects(NetworkMember{id}, action);
}

bool DataBaseNode::sendData(AbstractData *resp,
                            const QVariant &nodeId,
                            const Header *req) {


    if (!resp || !resp->prepareToSend()) {
        return false;
    }

    return sendData(const_cast<const AbstractData*>(resp), nodeId, req);
}

bool DataBaseNode::sendData(const AbstractData *resp,
                            const QVariant &nodeId,
                            const Header *req) {

    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value());
        if (info && info->id() == nodeId) {
            return sendData(resp, it.key(), req);
        }
    }

    return false;
}

bool DataBaseNode::sendData(const AbstractData *resp, const HostAddress &nodeId,
                            const Header *req) {
    return AbstractNode::sendData(resp, nodeId, req);

}

bool DataBaseNode::sendData(AbstractData *resp, const HostAddress &nodeId,
                            const Header *req) {
    return AbstractNode::sendData(resp, nodeId, req);
}

ParserResult DataBaseNode::parsePackage(const QSharedPointer<AbstractData> &pkg,
                                        const Header &pkgHeader,
                                        const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<WebSocket>() == pkg->cmd()) {
        WebSocket *obj = static_cast<WebSocket*>(pkg.data());

        QVariant requesterId = getSender(sender, obj);
        if (!obj->isValid()) {
            badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        if (!workWithSubscribe(*obj, requesterId, *sender)) {
            badRequest(sender->networkAddress(), pkgHeader, {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg->cmd()) {
        WebSocketSubscriptions *obj = static_cast<WebSocketSubscriptions*>(pkg.data());
        if (!obj->isValid()) {
            badRequest(sender->networkAddress(), pkgHeader, {
                           ErrorCodes::InvalidRequest,
                           "WebSocketSubscriptions request is invalid"
                       });
            return ParserResult::Error;
        }

        incomingData(obj, sender);
        return ParserResult::Processed;
    } else if (H_16<DeleteObject>() == pkg->cmd()) {
        auto obj = pkg.staticCast<DeleteObject>();

        auto requesterId = getSender(sender, obj.data());
        if (deleteObject(requesterId, obj) == DBOperationResult::Forbidden) {
            badRequest(sender->networkAddress(), pkgHeader, {
                           ErrorCodes::OperatioForbiden,
                           "Permision denied"
                       });
            return ParserResult::Error;

        }

    }

    return ParserResult::NotProcessed;
}

QByteArray DataBaseNode::hashgenerator(const QByteArray &pass) const {
    if (pass.isEmpty())
        return {};

    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

ISqlDBCache *DataBaseNode::db() const {
    return _db;
}

bool DataBaseNode::workWithSubscribe(const WebSocket &rec,
                                     const QVariant &clientOrNodeid,
                                     const AbstractNodeInfo & sender) {

    auto _db = db();
    if (!_db)
        return false;

    switch (static_cast<WebSocketRequest>(rec.getRequestCmd())) {

    case WebSocketRequest::Subscribe: {
        _webSocketWorker->subscribe(clientOrNodeid, rec.subscribeId());
        memberSubsribed(clientOrNodeid, rec.subscribeId());
        return true;
    }

    case WebSocketRequest::Unsubscribe: {
        _webSocketWorker->unsubscribe(clientOrNodeid, rec.subscribeId());
        memberUnsubsribed(clientOrNodeid, rec.subscribeId());
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

bool DataBaseNode::isForbidenTable(const QString &table) {
    return systemTables().contains(table);
}

QVariantMap DataBaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH + "/" + _localNodeName + "/" + _localNodeName + "_" + DEFAULT_DB_NAME},
    };
}

DBOperationResult
QH::DataBaseNode::getObject(const QVariant &requester,
                            const QH::DBObject &templateObj,
                            QSharedPointer<QH::PKG::DBObject> &result) const {

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

    result = obj;
    return DBOperationResult::Allowed;
}

DBOperationResult
DataBaseNode::getObjects(const QVariant &requester,
                         const DBObject &templateObj,
                         QList<QSharedPointer<DBObject>> &result) const {
    if (!_db) {
        return DBOperationResult::Unknown;
    }

    if (!_db->getAllObjects(templateObj, result)) {
        return DBOperationResult::Unknown;
    }

    for (const auto& obj: qAsConst(result)) {
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

DBOperationResult
DataBaseNode::updateObject(const QVariant &requester,
                           const QSharedPointer<DBObject> &saveObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = checkPermission(requester,
                                           saveObject->dbAddress(),
                                           Permission::Write);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    if (!_db->updateObject(saveObject)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

DBOperationResult
DataBaseNode::createObject(const QVariant &requester,
                           const QSharedPointer<DBObject> &obj) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    if (isForbidenTable(obj->tableName())) {
        return DBOperationResult::Forbidden;
    }

    if (!_db->insertObject(obj)) {
        return DBOperationResult::Unknown;
    }

    if (!addUpdatePermission(requester, obj->dbAddress(), Permission::Write)) {

        _db->deleteObject(obj);

        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;

}

DBOperationResult
DataBaseNode::updateIfNotExistsCreateObject(const QVariant &requester,
                                            const QSharedPointer<DBObject> &obj) {

    auto opResult = updateObject(requester, obj);
    if (opResult != QH::DBOperationResult::Unknown) {
        return opResult;
    }

    return createObject(requester, obj);
}

DBOperationResult
DataBaseNode::changeObjects(const QVariant &requester,
                            const DBObject &templateObj,
                            const std::function<bool (const QSharedPointer<DBObject> &)> &changeAction) {

    DBOperationResult result = DBOperationResult::Unknown;

    if (!_db) {
        return result;
    }

    auto execWithCheck = [this, requester, &result, &changeAction]
            (const QSharedPointer<DBObject> & obj) {

        result = checkPermission(requester, obj->dbAddress(), Permission::Update);
        if (result != DBOperationResult::Allowed) {
            return false;
        }

        return changeAction(obj);
    };

    if (!db()->changeObjects(templateObj, execWithCheck)) {
        return result;
    }

    return result;
}

QVariant DataBaseNode::getSender(const AbstractNodeInfo *connectInfo,
                                        const AbstractData *) const {

    auto info = dynamic_cast<const BaseNodeInfo*>(connectInfo);
    if (!info)
        return {};

    return info->id();
}

DBOperationResult
DataBaseNode::checkPermission(const QVariant &requester,
                              const DbAddress &objectAddress,
                              const Permission& requarimentPermision) const {

    if (!requester.isValid())
        return DBOperationResult::Unknown;

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto member = _db->getObjectRaw(NetworkMember{requester});
    if (!member) {
        return DBOperationResult::Unknown;
    }

    auto permision =
            _db->getObject(MemberPermisionObject({requester, objectAddress}));

    if (!permision) {
        return DBOperationResult::Unknown;
    }

    if (permision->permisions() < requarimentPermision) {
        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;
}

bool DataBaseNode::addUpdatePermission(const QVariant &member,
                                       const DbAddress &objectAddress,
                                       const Permission &permision) const {

    if (!_db) {
        return false;
    }

    auto object = QSharedPointer<MemberPermisionObject>::create();
    object->setKey(PermisionData(member, objectAddress));
    object->setPermisions(permision);

    if (!_db->insertObject(object) && !_db->updateObject(object)) {
        return false;
    }

    return true;

}

bool DataBaseNode::removePermission(const QVariant &member,
                                    const DbAddress &objectAddress) const {

    if (!_db) {
        return false;
    }

    auto object = QSharedPointer<MemberPermisionObject>::create();
    object->setKey(PermisionData(member, objectAddress));

    if (!_db->deleteObject(object)) {
        return false;
    }

    return true;
}

DBOperationResult
DataBaseNode::deleteObject(const QVariant &requester,
                           const QSharedPointer<DBObject> &dbObject) {

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

