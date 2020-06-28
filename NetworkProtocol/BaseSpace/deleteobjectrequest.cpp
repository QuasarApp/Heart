#include "deleteobjectrequest.h"
namespace NP {

DeleteObjectRequest::DeleteObjectRequest(const DbAddress &address):
    DBObject(address.table()) {

    INIT_COMMAND
    setId(address.id());
}

DeleteObjectRequest::DeleteObjectRequest(const Package &pkg):
    DBObject("") {
    fromBytes(pkg.toBytes());

}

DBObject *DeleteObjectRequest::factory() const {
    return new DeleteObjectRequest(DbAddress());
}

PrepareResult DeleteObjectRequest::prepareSelectQuery(QSqlQuery &q) const {
    Q_UNUSED(q)
    return PrepareResult::Disabled;
}

PrepareResult DeleteObjectRequest::prepareSaveQuery(QSqlQuery &q) const {
    Q_UNUSED(q)
    return PrepareResult::Disabled;
}

PrepareResult DeleteObjectRequest::prepareRemoveQuery(QSqlQuery &q) const {
    return DBObject::prepareRemoveQuery(q);
}
}
