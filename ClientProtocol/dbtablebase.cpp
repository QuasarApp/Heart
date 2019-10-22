#include "dbtablebase.h"

#include <QSet>

namespace ClientProtocol {

DbTableBase::DbTableBase(QString &name) {
    _name = name;
}

QString DbTableBase::name() const {
    return _name;
}

QString DbTableBase::toStringQuery() const {
    QString table = "CREATE TABLE IF NOT EXISTS" + _name + "(%0);";
    QString columns = " id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,";

    for (auto it = _tableMap.begin(); it != _tableMap.end(); ++it) {
        columns += it.key();

        auto val = it.value().toString();

        if (val.isEmpty()) {
            columns += "BLOB";
        } else {
            columns += val;
        }

        columns += ',';

    }

    return table.arg(columns);
}

QHash<QString, QVariant::Type> DbTableBase::keys() const {
    return _keys;
}

DbTableBase::~DbTableBase() {

}

QVariantMap DbTableBase::tableMap() const {
    return _tableMap;
}

void DbTableBase::setTableMap(const QVariantMap &tableMap) {
    _tableMap = tableMap;

    for (auto it = _tableMap.begin(); it != _tableMap.end(); ++it) {
        _keys[it.key()] = getType(it.value().toString());
    }
}

QHash<QString, QString> DbTableBase::dependencies() const {
    return _dependencies;
}

void DbTableBase::setDependencies(const QHash<QString, QString> &dependencies) {
    _dependencies = dependencies;
}

#define c(x) str.contains(x, Qt::CaseInsensitive)
QVariant::Type DbTableBase::getType(const QString &str) {

    if (str.isEmpty() || c(" BINARY") || c(" BLOB") || c(" TINYBLOB") || c(" MEDIUMBLOB") || c(" LONGBLOB")) {
        return QVariant::ByteArray;
    } else if (c(" INT")) {
        return QVariant::Int;
    } else if (c(" VARCHAR") || c(" TEXT") || c(" TINYTEXT") || c(" MEDIUMTEXT") || c(" LONGTEXT")) {
        return QVariant::String;
    } else if (c(" FLOAT") || c(" DOUBLE") || c(" REAL")) {
        return QVariant::Double;
    } else if (c(" BOOL")) {
        return QVariant::Bool;
    } else if (c(" DATETIME")) {
        return QVariant::DateTime;
    } else if (c(" DATE")) {
        return QVariant::Date;
    } else if (c(" TIME")) {
        return QVariant::Time;
    }

    return QVariant::ByteArray;
}

bool DbTableBase::addDependecies(const IDbTable *table, const QString &val) {

    if (table->keys().contains(val)) {
        _dependencies[table->name()] = val;
        return true;
    }

    return false;
}
}
