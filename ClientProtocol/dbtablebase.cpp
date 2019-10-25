#include "dbtablebase.h"

#include <QSet>

namespace ClientProtocol {

DbTableBase::DbTableBase(QString &name) {
    _name = name;
}

QString DbTableBase::name() const {
    return _name;
}

//QString DbTableBase::toStringQuery() const {
//    QString table = "CREATE TABLE IF NOT EXISTS" + _name + "(%0);";
//    QString columns = " id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,";
//    columns +=        " updateTime DATETIME NOT NULL DEFAULT NOW(),";

//    for (auto it = _tableMap.begin(); it != _tableMap.end(); ++it) {
//        columns += it.key();

//        auto val = it.value().toString();

//        if (val.isEmpty()) {
//            columns += "BLOB";
//        } else {
//            columns += val;
//        }

//        columns += ',';

//    }

//    for (auto it = _dependencies.begin(); it != _dependencies.end(); ++it) {
//        QString dep =   " FOREIGN KEY(_group) REFERENCES groups(id)
//        columns += "    FOREIGN KEY(_group) REFERENCES groups(id)"
//                ON UPDATE CASCADE
//                ON DELETE CASCADE,"
//    }

//    return table.arg(columns);
//}

QHash<QString, QVariant::Type> DbTableBase::keys() const {
    return _keys;
}

QString DbTableBase::lastIdQuery() const {
    return QString("SELECT MAX(id) FROM " + _name);
}

DbTableBase::~DbTableBase() {

}

void DbTableBase::setKeys(const QHash<QString, QVariant::Type> &keys) {
    _keys = keys;
}

}
