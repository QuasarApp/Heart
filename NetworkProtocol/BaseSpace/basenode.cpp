/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "accesstoken.h"
#include "basenode.h"
#include "basenodeinfo.h"
#include "dbdatarequest.h"
#include "deleteobjectrequest.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"
#include "permisions.h"

#include <badrequest.h>
#include <userdata.h>
#include <userrequest.h>
#include <quasarapp.h>
#include <transportdata.h>
#include <availabledatarequest.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <websocketcontroller.h>
#include <QCoreApplication>
#include <qsecretrsa2048.h>

namespace NP {

BaseNode::BaseNode(NP::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

    _webSocketWorker = new WebSocketController(this);

    _nodeKeys = new QSecretRSA2048();
    _nodeKeys->start();
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

ParserResult BaseNode::parsePackage(const Package &pkg,
                                    const AbstractNodeInfo *sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<BadRequest>() == pkg.hdr.command) {
        BadRequest cmd(pkg);
        emit requestError(cmd.err());
        emit incomingData(&cmd, sender->networkAddress());

        return ParserResult::Processed;

    } else if (H_16<TransportData>() == pkg.hdr.command) {
        TransportData cmd(pkg);

        if (cmd.address() == serverAddress()) {
            return parsePackage(cmd.data(), sender);
        }

        auto receiver = getInfoPtr(cmd.address());

        if (!receiver) {
            sendData(&cmd, receiver->networkAddress());
            return ParserResult::Processed;
        }

        return ParserResult::Processed;

    } else if (H_16<AvailableDataRequest>() == pkg.hdr.command) {
        AvailableDataRequest cmd(pkg);

        if (!cmd.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithAvailableDataRequest(&cmd, sender->networkAddress(), &pkg.hdr)) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<AvailableData>() == pkg.hdr.command) {
        AvailableData obj(pkg);
        if (!obj.isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(&obj, sender->networkAddress());
        return ParserResult::Processed;

    } else if (H_16<DeleteObjectRequest>() == pkg.hdr.command) {
        DeleteObjectRequest obj(pkg);

        DBOperationResult result = deleteObject(&obj, sender->networkAddress());

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
        auto obj = SP<WebSocket>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithSubscribe(obj, strongSender->networkAddress())) {
            badRequest(strongSender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg.hdr.command) {
        auto obj = SP<WebSocketSubscriptions>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->networkAddress());
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;

}

bool BaseNode::workWithAvailableDataRequest(const QWeakPointer<AbstractData> &rec,
                                            const QHostAddress &address,
                                            const Header *rHeader) {

    auto obj = rec.toStrongRef();
    if (obj.isNull())
        return false;

    auto info = getInfoPtr(address).toStrongRef().dynamicCast<BaseNodeInfo>();
    if (info.isNull())
        return false;

    auto av = SP<AvailableData>::create();
    av->setData({});

    return sendData(av, address, rHeader);

}

template <class RequestobjectType>
bool BaseNode::workWithDataRequest(const QWeakPointer<AbstractData> &rec,
                                   const QHostAddress &addere,
                                   const Header *rHeader) {

    auto request = rec.toStrongRef().dynamicCast<DBDataRequest>();

    if (request.isNull())
        return false;

    if (!isListening()) {
        return false;
    }

    auto _db = db().toStrongRef();

    if (_db.isNull()) {
        QuasarAppUtils::Params::log("Server not inited (db is null)",
                                    QuasarAppUtils::Error);
        return false;
    }

    switch (static_cast<DBDataRequestCmd>(request->getRequestCmd())) {
    case DBDataRequestCmd::Get: {

        auto obj = SP<RequestobjectType>::create().template dynamicCast<DBObject>();
        if (!getObject(obj, addere, request->address())) {
            return false;
        }

        if (!sendData(obj, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
                                        QuasarAppUtils::Warning);
            return false;
        }
        break;

    }

    case DBDataRequestCmd::Set: {

        if(!setObject(rec, addere, request->address())) {
            return false;
        }

        break;
    }

    default: return false;

    }

    return true;

}

QString BaseNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

AbstractNodeInfo *BaseNode::createNodeInfo(QAbstractSocket *socket) const {
    return  SP<BaseNodeInfo>::create(socket);
}

SqlDBCache *BaseNode::db() const {
    return _db;
}

// TO-DO
bool BaseNode::workWithSubscribe(const AbstractData *rec,
                                 const QHostAddress &address) {

    auto obj = rec.toStrongRef().dynamicCast<UserRequest>();
    if (obj.isNull())
        return false;

    auto info = getInfoPtr(address).toStrongRef();
    if (info.isNull())
        return false;

    auto _db = db().toStrongRef();

    if (_db.isNull())
        return false;

    switch (static_cast<WebSocketRequest>(obj->getRequestCmd())) {

    case WebSocketRequest::Subscribe: {
        return _webSocketWorker->subscribe(info, {obj->tableName(), obj->getId()});
    }

    case WebSocketRequest::Unsubscribe: {
        _webSocketWorker->unsubscribe(info, {obj->tableName(), obj->getId()});
        return true;
    }

    case WebSocketRequest::SubscribeList: {

        auto resp = SP<WebSocketSubscriptions>::create();
        resp->setAddresses(_webSocketWorker->list(info));

        return sendData(resp, address);
    }

    default: break;
    }

    return false;
}

DBOperationResult BaseNode::checkPermision(const AbstractNodeInfo *requestNode,
                              const DbAddress& object,
                              const int &requiredPermision) {


    auto node = dynamic_cast<const BaseNodeInfo*>(requestNode);
    if (!node) {
        return DBOperationResult::Forbidden;
    }

    if (!node->isHavePermisonRecord(object)) {
        return DBOperationResult::Unknown;
    }

    if (node->permision(object) < requiredPermision) {
        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;
}

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

DBOperationResult NP::BaseNode::getObject(DBObject *res,
                             const QHostAddress &requiredNodeAdderess,
                             const DbAddress& objcetAddress) {

    auto node = dynamic_cast<BaseNodeInfo*>(getInfoPtr(requiredNodeAdderess));
    if (node) {
        return DBOperationResult::Forbidden;
    }

    auto permision = checkPermision(node, objcetAddress, static_cast<int>(Permission::Read));
    if (permision != DBOperationResult::Allowed) {
        return permision;
    }

    if (!_db->getObject(res)) {
        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;
}

DBOperationResult BaseNode::setObject(const DBObject *saveObject,
                         const QHostAddress &requiredNodeAdderess,
                         const DbAddress &dbObject) {

    auto node = dynamic_cast<BaseNodeInfo*>(getInfoPtr(requiredNodeAdderess));
    if (node) {
        return DBOperationResult::Forbidden;
    }

    auto permision = checkPermision(node, dbObject, static_cast<int>(Permission::Write));
    if (permision != DBOperationResult::Allowed) {
        return permision;
    }

    if(!_db->saveObject(saveObject)) {
        QuasarAppUtils::Params::log("do not saved object in database!" + requiredNodeAdderess.toString(),
                                    QuasarAppUtils::Error);
        return DBOperationResult::Forbidden;
    }

    return DBOperationResult::Allowed;
}

DBOperationResult BaseNode::deleteObject(const AbstractData* rec,
                            const QHostAddress &addere) {

    auto request = dynamic_cast<const DeleteObjectRequest*>(rec);

    if (request)
        return DBOperationResult::Forbidden;

    if (!isListening()) {
        return DBOperationResult::Unknown;
    }

    if (_db) {
        QuasarAppUtils::Params::log("Server not inited (db is null)",
                                    QuasarAppUtils::Error);
        return DBOperationResult::Unknown;
    }

    auto node = dynamic_cast<BaseNodeInfo*>(getInfoPtr(addere));
    if (node) {
        return DBOperationResult::Forbidden;
    }

    auto permision = checkPermision(node, request->dbAddress(), static_cast<int>(Permission::Write));
    if (permision != DBOperationResult::Allowed) {
        return permision;
    }

    if(!_db->deleteObject(request)) {
        QuasarAppUtils::Params::log("do not saved object in database!" + addere.toString(),
                                    QuasarAppUtils::Error);
        return permision;
    }

    return DBOperationResult::Allowed;
}

}

