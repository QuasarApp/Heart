#include "deleteobjectrequest.h"
namespace NP {


DeleteObjectRequest::DeleteObjectRequest(const Package &pkg):
    DBObject("") {
    fromBytes(pkg.toBytes());

}

DeleteObjectRequest::DeleteObjectRequest(const DbAddress &address):
    DBObject(address.table) {

    INIT_COMMAND
    setId(address.id);
}

DBObject *DeleteObjectRequest::factory() {
    return new DeleteObjectRequest(DbAddress());
}

bool DeleteObjectRequest::select(QSqlQuery &q) {
    Q_UNUSED(q)
    return false;
}

bool DeleteObjectRequest::save(QSqlQuery &q) const {
    Q_UNUSED(q)
    return false;
}

bool DeleteObjectRequest::remove(QSqlQuery &q) const {
    return DBObject::remove(q);
}
}
