#include "dbobject.h"
#include "dbtablebase.h"
#include <QDataStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QHash>
#include <QSqlRecord>
#include <QVariantMap>

namespace ClientProtocol {

DBObject::DBObject(const QString &tableName) {
    _tableStruct.name = tableName;
}

DBObject::~DBObject() {

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
    auto map = QVariantMap{};
    auto dataMap = toVariantMap(map);

    for (auto it = _tableStruct.keys.begin(); it != _tableStruct.keys.end(); ++it) {
        retQuery += it.key() + ", ";
    }

    return true;
}

bool DBObject::generateSourceOfQuery(QString& retQuery,
                                     QList<QPair<QString, QVariant> > &retBindValue) const {
    retQuery = "";

    retBindValue.clear();

    auto dataMap = getMap();

    for (auto it = _tableStruct.keys.begin(); it != _tableStruct.keys.end(); ++it) {
        auto type = it.value();
        if (type != QVariant::UserType) {

            switch (type) {
            case QVariant::String:
            case QVariant::Int:
            case QVariant::Double:  {
                retQuery += "'" + dataMap.value(it.key()).toString() + "', ";
                break;
            }
            case QVariant::Time: {
                retQuery += "'" + dataMap.value(it.key()).toDate().toString("HH:mm:ss") + "', ";
                break;

            }
            case QVariant::DateTime: {
                retQuery += "'" + dataMap.value(it.key()).toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "', ";
                break;
            }
            case QVariant::Date: {
                retQuery += "'" + dataMap.value(it.key()).toDate().toString("yyyy-MM-dd") + "', ";
                break;
            }
            case QVariant::ByteArray: {
                auto bindValue = it.key() + "bytes";
                retQuery += ":" + bindValue + ", ";
                retBindValue.push_back({bindValue, dataMap.value(it.key())});

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

bool DBObject::setTableStruct(const DbTableBase &tableStruct) {
    auto map = getMap();


    for (auto it = tableStruct.keys.begin(); it != tableStruct.keys.end(); ++it) {
        if (!map.contains(it.key())) {
            return false;
        }
    }

    _tableStruct = tableStruct;

    return true;
}

QVariantMap DBObject::getMap() const {
    QVariantMap map;
    return toVariantMap(map);
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _tableStruct.name;
    stream >> _id;
    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _tableStruct.name;
    stream << _id;
    return stream;
}

QVariantMap &DBObject::fromVariantMap(QVariantMap &map) {
    return map;
}

QVariantMap &DBObject::toVariantMap(QVariantMap &map) const {
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
            _id > -1;
}

int DBObject::getId() const {
    return _id;
}

void DBObject::setId(int id) {
    _id = id;
}

void DBObject::clear() {
    _id = -1;
}


}
