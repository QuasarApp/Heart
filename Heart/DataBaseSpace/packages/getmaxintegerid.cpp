#include "getmaxintegerid.h"

#include <QSqlQuery>



namespace QH {
namespace PKG {


GetMaxIntegerId::GetMaxIntegerId(const QString& table, const QString& field) {

    _table = table;
    _field = field;
}

int GetMaxIntegerId::value() const {
    return _value;
}

DBObject *GetMaxIntegerId::createDBObject() const {
    return create<GetMaxIntegerId>(table(), _field);
}

PrepareResult GetMaxIntegerId::prepareSelectQuery(QSqlQuery &q) const {
    QString queryString = "SELECT max(%0) FROM %1";

    queryString = queryString.arg(_field, table());

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

QString GetMaxIntegerId::table() const {
    return _table;
}

QString GetMaxIntegerId::primaryKey() const {
    return "";
}

QString GetMaxIntegerId::primaryValue() const {
    return "";
}

}
}
