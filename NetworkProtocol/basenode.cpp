#include "accesstoken.h"
#include "basenode.h"
#include "basenodeinfo.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"

#include <badrequest.h>
#include <userdata.h>
#include <userdatarequest.h>
#include <quasarapp.h>
#include <transportdata.h>
#include <availabledatarequest.h>

namespace NetworkProtocol {

BaseNode::BaseNode(NetworkProtocol::SslMode mode, QObject *ptr):
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

ParserResult BaseNode::parsePackage(const Package &pkg,
                                    QWeakPointer<AbstractNodeInfo> sender) {
    auto parentResult = AbstractNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto strongSender = sender.toStrongRef();

    if (BadRequest().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<BadRequest>::create(pkg);
        emit requestError(cmd->err());

        return ParserResult::Processed;

    } else if (TransportData().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<TransportData>::create(pkg);

        if (cmd->address() == serverAddress()) {
            return parsePackage(cmd->data(), sender);
        }

        auto receiver = getInfoPtr(cmd->address()).toStrongRef();

        if (!receiver.isNull()) {
            sendData(cmd, receiver->id());
            return ParserResult::Processed;
        }

        return ParserResult::Processed;

    } else if (AvailableDataRequest().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<AvailableDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithAvailableDataRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (AvailableData().cmd() == pkg.hdr.command) {
        auto obj = QSharedPointer<AvailableData>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
        return ParserResult::Processed;

    }

    return ParserResult::NotProcessed;

}

bool BaseNode::workWithAvailableDataRequest(QWeakPointer<AvailableDataRequest> rec,
                                            const QHostAddress &address,
                                            const Header *rHeader) {

    auto obj = rec.toStrongRef();
    if (obj.isNull())
        return false;

    auto info = getInfoPtr(address).toStrongRef().dynamicCast<BaseNodeInfo>();
    if (info.isNull())
        return false;

    auto av = QSharedPointer<AvailableData>::create();
    av->setData({});

    return sendData(av, address, rHeader);

}

QString BaseNode::hashgenerator(const QByteArray &pass) {
    return QCryptographicHash::hash(
                QCryptographicHash::hash(pass, QCryptographicHash::Sha256) + "QuassarAppSoult",
                QCryptographicHash::Sha256);
}

QSharedPointer<AbstractNodeInfo> BaseNode::createNodeInfo(QAbstractSocket *socket) const {
    return  QSharedPointer<BaseNodeInfo>::create(socket);
}

QWeakPointer<SqlDBCache> BaseNode::db() const {
    return _db;
}

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

}

