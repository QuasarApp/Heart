#include "basenode.h"
#include "basenodeinfo.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"

#include <badrequest.h>
#include <userdata.h>
#include <userdatarequest.h>
#include <quasarapp.h>

namespace ClientProtocol {

BaseNode::BaseNode(ClientProtocol::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

}

bool BaseNode::intSqlDb(QString DBparamsFile,
                        SqlDBCache *cache,
                        SqlDBWriter *writer) {

    initDefaultDbObjects(cache, writer);

    if (!_db->init(DBparamsFile)) {
        return false;
    }

    return true;
}

bool BaseNode::intSqlDb(QVariantMap params, SqlDBCache *cache, SqlDBWriter *writer) {

    initDefaultDbObjects(cache, writer);

    if (params.isEmpty()) {
        params = defaultDbParams();
    }

    if (!_db->init(params)) {
        return false;
    }

    return true;
}

bool BaseNode::isSqlInited() const {
    return !_db.isNull();
}

bool BaseNode::run(const QString &addres, unsigned short port) {
    if (!isSqlInited() && !intSqlDb()) {
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

    cache->setWriter(QSharedPointer<SqlDBWriter>(writer));
    _db = QSharedPointer<SqlDBCache>(cache);
}

bool BaseNode::parsePackage(const Package &pkg, QWeakPointer<AbstractNodeInfo> sender) {
    if (!AbstractNode::parsePackage(pkg, sender)) {
        return false;
    }

    auto strongSender = sender.toStrongRef();

    if (BadRequest().cmd() == pkg.hdr.command) {
        emit requestError();
    } else if (UserDataRequest().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<UserDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return false;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
        }

    }

    return true;

}

bool BaseNode::workWithUserRequest(QWeakPointer<UserDataRequest> rec, const QHostAddress &addere,
                                   const Header *rHeader) {
    auto request = rec.toStrongRef();

    if (request.isNull())
        return false;

    if (!isListening()) {
        return false;
    }

    if (_db.isNull()) {
        QuasarAppUtils::Params::verboseLog("Server not inited (db is null)",
                                           QuasarAppUtils::Error);
        return false;
    }

    switch (request->requestCmd()) {
    case UserDataRequestCmd::Get: {

        QSharedPointer<DBObject> res;
        if (!_db->getObject(request->tableName(), request->getId(), &res)) {
            return false;
        }

        if (!sendResponse(res, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }
        break;

    }

    case UserDataRequestCmd::Save: {

        if(!_db->saveObject(request)) {
            QuasarAppUtils::Params::verboseLog("do not saved object in database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }

        if (!sendResponse(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

        // TODO

    case UserDataRequestCmd::Login: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        QSharedPointer<DBObject> res;
        if (!_db->getObject(request->tableName(), request->getId(), &res)) {
            return false;
        }

        auto user = res.dynamicCast<UserData>();
        if (user.isNull()) {
            return false;
        }

        if (user->passSHA256() == request->passSHA256()) {

        }


        //node->setToken();

        if (!sendResponse(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

    case UserDataRequestCmd::Delete: {

        if(!_db->deleteObject(request->tableName(), request->getId())) {
            QuasarAppUtils::Params::verboseLog("do not deleted object from database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }


        if (!sendResponse(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
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

QSharedPointer<AbstractNodeInfo> BaseNode::createNodeInfo(QAbstractSocket *socket) const {
    return  QSharedPointer<BaseNodeInfo>::create(socket);
}

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

}

