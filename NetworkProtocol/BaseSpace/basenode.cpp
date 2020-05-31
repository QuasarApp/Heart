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
#include "sqldbcache.h"
#include "sqldbwriter.h"
#include "websocketcontroller.h"

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

namespace NP {

BaseNode::BaseNode(NP::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

    _webSocketWorker = new WebSocketController(this);
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
    return !_db.isNull();
}

bool BaseNode::run(const QString &addres, unsigned short port) {
    if (!isSqlInited() && !initSqlDb()) {
        return false;
    }

    return AbstractNode::run(addres, port);
}

BaseNode::~BaseNode() {

}

void BaseNode::initDefaultDbObjects(SqlDBCache *cache, SqlDBWriter *writer) {
    if (!writer) {
        writer = new SqlDBWriter();
    }

    if (!cache) {
        cache = new SqlDBCache();
    }

    cache->setWriter(SP<SqlDBWriter>(writer));
    _db = SP<SqlDBCache>(cache);

    connect(_db.data(), &SqlDBCache::sigItemChanged,
            _webSocketWorker, &WebSocketController::handleItemChanged);
}

ParserResult BaseNode::parsePackage(const Package &pkg,
                                    const WP<AbstractNodeInfo>& sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto strongSender = sender.toStrongRef();

    if (H_16<BadRequest>() == pkg.hdr.command) {
        auto cmd = SP<BadRequest>::create(pkg);
        emit requestError(cmd->err());
        emit incomingData(cmd, strongSender->id());

        return ParserResult::Processed;

    } else if (H_16<TransportData>() == pkg.hdr.command) {
        auto cmd = SP<TransportData>::create(pkg);

        if (cmd->address() == serverAddress()) {
            return parsePackage(cmd->data(), sender);
        }

        auto receiver = getInfoPtr(cmd->address()).toStrongRef();

        if (!receiver.isNull()) {
            sendData(cmd, receiver->id());
            return ParserResult::Processed;
        }

        return ParserResult::Processed;

    } else if (H_16<AvailableDataRequest>() == pkg.hdr.command) {
        auto cmd = SP<AvailableDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithAvailableDataRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<AvailableData>() == pkg.hdr.command) {
        auto obj = SP<AvailableData>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
        return ParserResult::Processed;

    } else if (H_16<WebSocket>() == pkg.hdr.command) {
        auto obj = SP<WebSocket>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithSubscribe(obj, strongSender->id())) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg.hdr.command) {
        auto obj = SP<WebSocketSubscriptions>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
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

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (!checkPermision(node.data(), request->address(), Permission::Read)) {
            return false;
        }

        auto res = SP<RequestobjectType>::create().template dynamicCast<DBObject>();
        if (!_db->getObject(res)) {
            return false;
        }

        if (!sendData(res, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
                                        QuasarAppUtils::Warning);
            return false;
        }
        break;

    }

    case DBDataRequestCmd::Set: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (!checkPermision(node.data(), request->address(), Permission::Write)) {
            return false;
        }

        if(!_db->saveObject(rec)) {
            QuasarAppUtils::Params::log("do not saved object in database!" + addere.toString(),
                                        QuasarAppUtils::Error);
            return false;
        }

        break;
    }

    case DBDataRequestCmd::Delete: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (!checkPermision(node.data(), request->address(), Permission::Write)) {
            return false;
        }

        if(!_db->deleteObject(rec)) {
            QuasarAppUtils::Params::log("do not saved object in database!" + addere.toString(),
                                        QuasarAppUtils::Error);
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

SP<AbstractNodeInfo> BaseNode::createNodeInfo(QAbstractSocket *socket) const {
    return  SP<BaseNodeInfo>::create(socket);
}

WP<SqlDBCache> BaseNode::db() const {
    return _db;
}

// TO-DO
bool BaseNode::workWithSubscribe(const WP<AbstractData> &rec,
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

bool BaseNode::checkPermision(const AbstractNodeInfo *requestNode,
                              const DbAddress& object,
                              const Permission &requiredPermision) {

    auto node = dynamic_cast<const BaseNodeInfo*>(requestNode);
    if (!node) {
        return false;
    }

    if (node->permision(object) < requiredPermision) {
        return false;
    }

    return true;
}

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

}

