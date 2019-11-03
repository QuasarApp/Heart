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
    _tableName = tableName;
}

DBObject::~DBObject() {

}

QVariantMap DBObject::getMap() const {
    QVariantMap map;
    return toVariantMap(map);
}

void DBObject::setMap(const QVariantMap& map) {
    fromVariantMap(map);
}

QString DBObject::tableName() const {
    return _tableName;
}

void DBObject::setTableName(const QString &tableName) {
    _tableName = tableName;
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _tableName;
    stream >> _id;
    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _tableName;
    stream << _id;
    return stream;
}

void DBObject::fromVariantMap(const QVariantMap &map) {
    if (map.contains("id")) {
        _id = map.value("id").toInt();
    }

    return;
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
    return AbstractData::isValid() && _tableName.size() &&
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
