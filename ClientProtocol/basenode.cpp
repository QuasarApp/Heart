#include "basenode.h"
#include "sqldbcache.h"
#include "sqldbwriter.h"

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

QVariantMap BaseNode::defaultDbParams() const {

    return {
        {"DBDriver", "QSQLITE"},
        {"DBFilePath", DEFAULT_DB_PATH},
        {"DBInitFile", DEFAULT_DB_INIT_FILE_PATH}
    };
}

}

