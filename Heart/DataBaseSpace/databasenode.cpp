/*
 * Copyright (C) 2018-2022 QuasarApp.
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
#include <basenodeinfo.h>
#include <abstractnetworkmember.h>
#include <memberpermisionobject.h>
#include <networkmember.h>
#include <deleteobject.h>
#include "dberrorcodes.h"
#include <QSet>
#include <defaultpermision.h>
#include <isubscribabledata.h>
#include <itoken.h>
#include <sqlitedbcache.h>
#include <sqldb.h>
#include <QCryptographicHash>
#include "getsinglevalue.h"
#include "setsinglevalue.h"
#include "database.h"

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

bool DataBaseNode::initDatabase() {
    setDb(new DataBase());
    return true;
}

void DataBaseNode::setDb(DataBase *newDb) {
    if (_db) {
        disconnect(_db, &DataBase::sigObjectChanged,
                   this, &DataBaseNode::handleObjectChanged);

        disconnect(_db, &DataBase::sigObjectDeleted,
                   this, &DataBaseNode::handleObjectDeleted);

        delete _db;
    }

    _db = newDb;

    if (_db) {
        connect(_db, &DataBase::sigObjectChanged,
                this, &DataBaseNode::handleObjectChanged,
                Qt::DirectConnection);

        connect(_db, &DataBase::sigObjectDeleted,
                this, &DataBaseNode::handleObjectDeleted,
                Qt::DirectConnection);
    }
}


bool DataBaseNode::isSqlInited() const {
    return _db && _db->isSqlInited();
}

DBOperationResult DataBaseNode::checkPermission(const QString &requester,
                                                const DbAddress &objectAddress,
                                                const Permission &requarimentPermision) const {
    if (!_db) {
        return DBOperationResult::Unknown;
    }

    return _db->checkPermission(requester, objectAddress, requarimentPermision);
}

bool DataBaseNode::run(const QString &addres, unsigned short port) {
    return initDatabase() && _db->run() &&AbstractNode::run(addres, port);
}

bool DataBaseNode::run(const QString &addres,
                       unsigned short port,
                       const QString &localNodeName) {
    return initDatabase() && _db->run(localNodeName) && AbstractNode::run(addres, port);
}

void DataBaseNode::stop() {

    AbstractNode::stop();

    if (_db)
        _db->stop();
}

DataBaseNode::~DataBaseNode() {
    if (_db)
        delete _db;
}

bool DataBaseNode::welcomeAddress(AbstractNodeInfo *) {
    return true;
}

bool DataBaseNode::isBanned(const QString &node) const {
    return db()->isBanned(node);
}

bool DataBaseNode::isBanned(const AbstractNodeInfo *node) const {
    return AbstractNode::isBanned(node);
}

bool DataBaseNode::notifyObjectChanged(const QSharedPointer<PKG::ISubscribableData> &item) {

    if (!item.dynamicCast<PKG::AbstractData>()) {
        return false;
    }

    _webSocketWorker->handleItemChanged(item);

    return true;
}

void DataBaseNode::objectRemoved(const DbAddress &) {

}

void DataBaseNode::objectChanged(const QSharedPointer<DBObject> &) {

}

void DataBaseNode::handleObjectChanged(const QSharedPointer<DBObject> &item) {
    notifyObjectChanged(item.staticCast<PKG::ISubscribableData>());
    objectChanged(item);

}

void DataBaseNode::handleObjectDeleted(const DbAddress &item) {
    objectRemoved(item);
}

void DataBaseNode::nodeConnected(AbstractNodeInfo *node) {
    AbstractNode::nodeConnected(node);
    welcomeAddress(node);
}

void DataBaseNode::nodeDisconnected(AbstractNodeInfo *node) {
    AbstractNode::nodeDisconnected(node);

    auto baseInfo = dynamic_cast<BaseNodeInfo*>(node);

    if (baseInfo) {
        baseInfo->reset();
    }
}

void DataBaseNode::memberSubsribed(const QVariant &, unsigned int ) {
    return;
}

void DataBaseNode::memberUnsubsribed(const QVariant &, unsigned int ) {
    return;
}

QString DataBaseNode::dbLocation() const {
    if (!db()) {
        return "";
    }

    return db()->dbLocation();
}

AbstractNodeInfo *DataBaseNode::createNodeInfo(QAbstractSocket *socket, const HostAddress *clientAddress) const {
    return new BaseNodeInfo(socket, clientAddress);
}

bool DataBaseNode::changeTrust(const HostAddress &id, int diff) {    
    auto info = dynamic_cast<const BaseNodeInfo*>(getInfoPtr(id));
    if (!info)
        return false;

    if (info->id().size())
        return changeTrust(info->id(), diff);

    return AbstractNode::changeTrust(id, diff);
}

bool DataBaseNode::changeTrust(const QString &id, int diff) {
    if (!_db)
        return false;
    return _db->changeTrust(id, diff);
}

unsigned int DataBaseNode::sendData(const AbstractData *resp,
                                    const QVariant &nodeId,
                                    const Header *req) {

    auto nodes = connections();

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto info = dynamic_cast<BaseNodeInfo*>(it.value());
        if (info && info->id() == nodeId) {
            return sendData(resp, info, req);
        }
    }

    return 0;
}

unsigned int DataBaseNode::sendData(const AbstractData *resp, const HostAddress &nodeId,
                                    const Header *req) {
    return AbstractNode::sendData(resp, nodeId, req);

}

unsigned int DataBaseNode::sendData(const PKG::AbstractData *resp,
                                    const AbstractNodeInfo *node,
                                    const Header *req) {
    return AbstractNode::sendData(resp, node, req);
}

ParserResult DataBaseNode::parsePackage(const QSharedPointer<AbstractData> &pkg,
                                        const Header &pkgHeader,
                                        const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (WebSocket::command() == pkg->cmd()) {
        WebSocket *obj = static_cast<WebSocket*>(pkg.data());

        auto requesterId = getSender(sender, obj);
        if (!obj->isValid()) {
            badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        if (!workWithSubscribe(*obj, requesterId, sender)) {
            badRequest(sender->networkAddress(), pkgHeader, {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        return ParserResult::Processed;

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

DataBase *DataBaseNode::db() const {
    return _db;
}

bool DataBaseNode::workWithSubscribe(const WebSocket &rec,
                                     const QString &clientOrNodeid,
                                     const AbstractNodeInfo * sender) {

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

        return sendData(&resp, sender);
    }

    default: break;
    }

    return false;
}

DBOperationResult
QH::DataBaseNode::getObject(const QString &requester,
                            const QH::DBObject &templateObj,
                            QSharedPointer<QH::PKG::DBObject> &result) const {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->getObject(requester, templateObj, result);
}

DBOperationResult
DataBaseNode::getObjects(const QString &requester,
                         const DBObject &templateObj,
                         QList<QSharedPointer<DBObject>> &result) const {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->getObjects(requester, templateObj, result);
}

DBOperationResult
DataBaseNode::updateObject(const QString &requester,
                           const QSharedPointer<DBObject> &saveObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->updateObject(requester, saveObject);
}

DBOperationResult
DataBaseNode::createObject(const QString &requester,
                           const QSharedPointer<DBObject> &obj) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->createObject(requester, obj);
}

DBOperationResult
DataBaseNode::updateIfNotExistsCreateObject(const QString &requester,
                                            const QSharedPointer<DBObject> &obj) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->updateIfNotExistsCreateObject(requester, obj);
}

DBOperationResult
DataBaseNode::changeObjects(const QString &requester,
                            const DBObject &templateObj,
                            const std::function<bool (const QSharedPointer<DBObject> &)> &changeAction) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->changeObjects(requester, templateObj, changeAction);
}

QString DataBaseNode::getSender(const AbstractNodeInfo *connectInfo,
                                 const AbstractData *) const {

    auto info = dynamic_cast<const BaseNodeInfo*>(connectInfo);
    if (!info)
        return {};

    return info->id();
}

DBOperationResult
DataBaseNode::deleteObject(const QString &requester,
                           const QSharedPointer<DBObject> &dbObject) {

    if (!_db) {
        return DBOperationResult::Unknown;
    }
    return _db->deleteObject(requester, dbObject);
}

}

