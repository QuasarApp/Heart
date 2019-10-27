#include "dbobject.h"
#include "dbtablebase.h"
#include <QDataStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QHash>
#include <QSqlRecord>

namespace ClientProtocol {

DBObject::DBObject(const QString &tableName) {
    _tableStruct.name = tableName;
}

// 0 - table name
// 1 - headers of update values
// 2 - update values
bool DBObject::saveQuery(QSqlQuery *query) const {

    QString queryString = "INSERT IGNORE INTO %0(%1) VALUES (%2)";
    if (!getBaseQueryString(queryString, query)) {
        return false;
    }

    return exec(query);
}

bool DBObject::selectQuery(QSqlQuery *query) {

    if (getId() < 0) {
        return false;
    }

    QString queryString = "SELECT (%1) from %0 where id=" + QString::number(getId());

    if (!getBaseQueryString(queryString, query)) {
        return false;
    }

    if (! exec(query)) {
        return false;
    }

    QSqlRecord record = query->record();
    QVariantMap initMap;

    for (int i = 0; i < query->size(); ++i ) {
        initMap[record.fieldName(i)] = record.value(i);
    }

    fromVariantMap(initMap);

    return isValid();
}

bool DBObject::deleteQuery(QSqlQuery *query) const {
    if (getId() < 0) {
        return false;
    }

    QString queryString = "DELETE FROM %0 where id=" + QString::number(getId());

    if (!getBaseQueryString(queryString, query)) {
        return false;
    }

    return exec(query);
}

bool DBObject::generateHeaderOfQuery(QString & retQuery) const {
    retQuery = "";
    for (auto it = _dataTable.begin(); it != _dataTable.end(); ++it) {
        if (_tableStruct.keys.contains(it.key())) {
            retQuery += it.key() + ", ";
        }
    }

    return true;
}

bool DBObject::generateSourceOfQuery(QString& retQuery,
                                     QList<QPair<QString, QVariant> > &retBindValue) const {
    retQuery = "";

    retBindValue.clear();
    for (auto it = _dataTable.begin(); it != _dataTable.end(); ++it) {
        auto type = _tableStruct.keys.value(it.key(), QVariant::UserType);
        if (type != QVariant::UserType) {

            switch (type) {
            case QVariant::String:
            case QVariant::Int:
            case QVariant::Double:  {
                retQuery += "'" + it.value().toString() + "', ";
                break;
            }
            case QVariant::Time: {
                retQuery += "'" + it.value().toDate().toString("HH:mm:ss") + "', ";
                break;

            }
            case QVariant::DateTime: {
                retQuery += "'" + it.value().toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "', ";
                break;
            }
            case QVariant::Date: {
                retQuery += "'" + it.value().toDate().toString("yyyy-MM-dd") + "', ";
                break;
            }
            case QVariant::ByteArray: {
                auto bindValue = it.key() + "bytes";
                retQuery += ":" + bindValue + ", ";
                retBindValue.push_back({bindValue, it.value()});

                break;
            }
            default: {
                break;
            }

            }

        }
    }

    return true;
}

bool DBObject::getBaseQueryString(QString queryString, QSqlQuery *query) const {

    queryString = queryString.arg(_tableStruct.name);

    QString temp = "";
    if (!generateHeaderOfQuery(temp)) {
        return false;
    }

    queryString = queryString.arg(temp);

    temp = "";
    QList<QPair<QString, QVariant> > bindValues;

    if (!generateSourceOfQuery(temp, bindValues)) {
        return false;
    }

    queryString = queryString.arg(temp);

    if (!query->prepare(queryString)) {
        return false;
    }

    for (auto &i: bindValues) {
        query->bindValue(i.first, i.second);
    }

    return true;
}

DbTableBase DBObject::getTableStruct() const {
    return _tableStruct;
}

void DBObject::setTableStruct(const DbTableBase &tableStruct) {
    _tableStruct = tableStruct;
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _dataTable;
    stream >> _tableStruct.name;
    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _dataTable;
    stream << _tableStruct.name;
    return stream;
}

QVariantMap &DBObject::fromVariantMap(QVariantMap &map) {
    _dataTable = map;
    return map;
}

QVariantMap &DBObject::toVariantmap(QVariantMap &map) const {
    map = _dataTable;
    return map;
}

bool DBObject::exec(QSqlQuery *query) const {
    if (!query->exec()) {
        return false;
    }

    if (!query->next()) {
        return false;
    }

    return true;
}

bool DBObject::isValid() const {
    return AbstractData::isValid() && _tableStruct.isValid() &&
            _dataTable.contains("id");
}

int DBObject::getId() const {
    return _dataTable.value("id").toInt();
}

void DBObject::setId(int id) {
    _dataTable["id"] = id;
}

void DBObject::clear() {
    _dataTable.clear();
}


}
