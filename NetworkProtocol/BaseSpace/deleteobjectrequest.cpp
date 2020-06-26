#include "deleteobjectrequest.h"
namespace NP {


DeleteObjectRequest::DeleteObjectRequest(const Package &pkg):
    DBObject("") {
    fromBytes(pkg.toBytes());

}

DeleteObjectRequest::DeleteObjectRequest(const DbAddress &address):
    DBObject(address.table()) {

    INIT_COMMAND
    setId(address.id());
}

DBObject *DeleteObjectRequest::factory() const {
    return new DeleteObjectRequest(DbAddress());
}

bool DeleteObjectRequest::prepareSelectQuery(QSqlQuery &q) const {
    Q_UNUSED(q)
    return false;
}

bool DeleteObjectRequest::prepareSaveQuery(QSqlQuery &q) const {
    Q_UNUSED(q)
    return false;
}

bool DeleteObjectRequest::prepareRemoveQuery(QSqlQuery &q) const {
    return DBObject::prepareRemoveQuery(q);
}
}
