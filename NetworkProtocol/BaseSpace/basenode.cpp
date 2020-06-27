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
#include "nodeobject.h"
#include "nodespermisionobject.h"

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
        emit incomingData(&cmd, pkg.hdr.sender);

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
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithAvailableDataRequest(&cmd, &pkg.hdr)) {
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<AvailableData>() == pkg.hdr.command) {
        AvailableData obj(pkg);
        if (!obj.isValid()) {
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(&obj, pkg.hdr.sender);
        return ParserResult::Processed;

    } else if (H_16<DeleteObjectRequest>() == pkg.hdr.command) {
        DeleteObjectRequest obj(pkg);

        DBOperationResult result = deleteObject(&obj, pkg.hdr.sender);

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
        if (!obj.isValid()) {
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithSubscribe(obj, pkg.hdr.sender)) {
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    } else if (H_16<WebSocketSubscriptions>() == pkg.hdr.command) {
        WebSocketSubscriptions obj(pkg);
        if (!obj.isValid()) {
            badRequest(pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(&obj, pkg.hdr.sender);
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;

}

bool BaseNode::workWithAvailableDataRequest(const AbstractData *rec,
                                            const Header *rHeader) {

    if (!rec)
        return false;

    auto info = getObject(NodeObject(rHeader->sender));
    if (!info)
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
bool BaseNode::workWithSubscribe(const WebSocket &rec,
                                 const DbId &clientOrNodeid) {


    NodeObject *node = getObject(NodeObject{clientOrNodeid});

    if (node)
        return false;

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
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

bool BaseNode::sendData(const AbstractData *resp,
                        const QHostAddress &addere,
                        const Header *req) {

    return AbstractNode::sendData(resp, addere, req);
}

bool BaseNode::sendData(const AbstractData *resp,
                        const DbId &nodeId,
                        const Header *req) {
    To Do

            //    auto client = getInfoPtr(addere);

            //    if (client) {
            //        QuasarAppUtils::Params::log("Response not sent because client == null",
            //                                           QuasarAppUtils::Error);
            //        return false;
            //    }

            //    if (resp) {
            //        return false;
            //    }

            //    Package pkg;
            //    bool convert = false;
            //    if (req) {
            //        convert = resp->toPackage(pkg, req->command);
            //    } else {
            //        convert = resp->toPackage(pkg);
            //    }

            //    if (!convert) {
            //        QuasarAppUtils::Params::log("Response not sent because dont create package from object",
            //                                           QuasarAppUtils::Error);
            //        return false;
            //    }


            //    if (!sendPackage(pkg, client->sct())) {
            //        QuasarAppUtils::Params::log("Response not sent!",
            //                                           QuasarAppUtils::Error);
            //        return false;
            //    }

            //    return true;
}

void BaseNode::badRequest(const QHostAddress &address, const Header &req, const QString msg) {
    AbstractNode::badRequest(address, req, msg);
}

void BaseNode::badRequest(const Header &req, const QString msg) {
    auto client = getObject(NodeObject{req.sender});

    if (!client) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because client == null",
                                    QuasarAppUtils::Error);
        return;
    }

    if (!changeTrust(req.sender, REQUEST_ERROR)) {

        QuasarAppUtils::Params::log("Bad request detected, bud responce command not sendet!"
                                    " because trust not changed",
                                    QuasarAppUtils::Error);

        return;
    }

    auto bad = BadRequest(msg);
    if (!sendData(&bad, req.sender, &req)) {
        return;
    }

    QuasarAppUtils::Params::log("Bad request sendet to adderess: " +
                                req.sender.toBase64(),
                                QuasarAppUtils::Info);
}

bool BaseNode::changeTrust(const QHostAddress &id, int diff) {
    return AbstractNode::changeTrust(id, diff);
}

bool BaseNode::changeTrust(const DbId &id, int diff) {
    To Do
}

DBOperationResult NP::BaseNode::getObject(const NP::DbId &requester,
                                          const DbAddress& objcetAddress,
                                          const NP::DBObject *res) const {

    if (!_db) {
        return DBOperationResult::Unknown;
    }

    auto permisionResult = _db->checkPermision(requester, objcetAddress, Permission::Read);
    if (permisionResult != DBOperationResult::Allowed) {
        return permisionResult;
    }

    if (!_db->getObject(res)) {
        return DBOperationResult::Unknown;
    }

    return DBOperationResult::Allowed;
}

DBOperationResult BaseNode::setObject(const DbId &requester,
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

DBOperationResult BaseNode::deleteObject(const DbId &requester,
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

