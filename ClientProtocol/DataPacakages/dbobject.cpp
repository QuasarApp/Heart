#include "dbobject.h"
#include "dbtablebase.h"
#include <QDataStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QHash>

namespace ClientProtocol {

DBObject::DBObject(const QString &tableName) {
    _tableStruct.name = tableName;
}

// 0 - table name
// 1 - headers of update values
// 2 - update values
bool DBObject::getSaveQueryString(QSqlQuery *query) const {

    QString queryString = "INSERT IGNORE INTO %0(%1) VALUES (%2)";
    return getBaseQueryString(queryString, query);
}

bool DBObject::getSelectQueryString(QSqlQuery *query) const {

    if (_id < 0) {
        return false;
    }

    QString queryString = "SELECT (%1) from %0 where id=" + QString::number(_id);
    return getBaseQueryString(queryString, query);
}

bool DBObject::getDeleteQueryString(QSqlQuery *query) const {
    if (_id < 0) {
        return false;
    }

    QString queryString = "DELETE FROM %0 where id=" + QString::number(_id);
    return getBaseQueryString(queryString, query);
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

QDataStream &DBObject::fromStream(QDataStream &stream) {
    stream >> _dataTable;
    stream >> _tableStruct.name;
    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {

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

bool DBObject::isValid() const {
    return AbstractData::isValid() && _tableStruct.isValid();
}

int DBObject::getId() const {
    return _id;
}


}
