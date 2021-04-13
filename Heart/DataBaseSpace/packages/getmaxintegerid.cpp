#include "getmaxintegerid.h"

#include <QSqlQuery>



namespace QH {
namespace PKG {


GetMaxIntegerId::GetMaxIntegerId(const QString& table, const QString& field):
    DBObject(table) {

    _field = field;
}

int GetMaxIntegerId::value() const {
    return _value;
}

DBObject *GetMaxIntegerId::createDBObject() const {
    return create<GetMaxIntegerId>(tableName(), _field);
}

PrepareResult GetMaxIntegerId::prepareSelectQuery(QSqlQuery &q) const {
    QString queryString = "SELECT max(%0) FROM %1";

    queryString = queryString.arg(_field, tableName());

    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

bool GetMaxIntegerId::fromSqlRecord(const QSqlRecord &q) {
    _value = q.value(0).toInt();

    return true;
}

bool GetMaxIntegerId::isCached() const {
    return false;
}

QString GetMaxIntegerId::primaryKey() const {
    return "";
}

}
}
