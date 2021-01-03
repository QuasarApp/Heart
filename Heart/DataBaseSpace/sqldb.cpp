#include "sqldb.h"
namespace QH {

SqlDB::SqlDB():
    ISqlDBCache(0, SqlDBCasheWriteMode::Force) {
}

void SqlDB::deleteFromCache(const QSharedPointer<PKG::DBObject> &delObj) {
    Q_UNUSED(delObj);
}

bool SqlDB::insertToCache(const QSharedPointer<PKG::DBObject> &obj) {
    Q_UNUSED(obj);
    return false;
}

bool SqlDB::updateCache(const QSharedPointer<PKG::DBObject> &obj) {
    Q_UNUSED(obj);
    return false;
}

QList<QSharedPointer<PKG::DBObject>>
SqlDB::getFromCache(const PKG::DBObject *obj) {
    Q_UNUSED(obj);
    return {};
}
}
