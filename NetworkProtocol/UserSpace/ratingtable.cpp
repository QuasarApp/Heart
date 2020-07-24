/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "ratingtable.h"

#include <QDataStream>
#include <QSharedPointer>
#include <QSqlQuery>

namespace NP {

int RatingTable::id = 0;
QString RatingTable::table = "users";


RatingTable::RatingTable():DBObjectQuery(table) {
    INIT_COMMAND
    setId(id);
}
RatingTable::RatingTable(const Package &package):RatingTable() {
    fromBytes(package.data);
}

QDataStream &RatingTable::fromStream(QDataStream &stream) {
    DBObjectQuery::fromStream(stream);

    stream >> _data;

    return stream;
}

QDataStream &RatingTable::toStream(QDataStream &stream) const {
    DBObjectQuery::toStream(stream);

    stream << _data;

    return stream;
}

DBObject *RatingTable::factory() {
    return new RatingTable();
}

bool RatingTable::select(QSqlQuery &q) {

    if (!isUpdateAllowed()) {
        return false;
    }

    QString query;

    query = "SELECT name, points from '" + tableName() +
            " ORDER BY points LIMIT 32000";

    if (!q.prepare(query))
        return false;

    if (!q.exec())
        return false;

    _data.clear();

    short number = 0;
    while (q.next()) {
        UserRecord rec;

        rec.setName(q.value("name").toString());
        rec.setRecord(static_cast<short>(q.value("points").toInt()));

        _data.insert(++number, rec);
    }
    return isValid();
}

bool RatingTable::save(QSqlQuery &) const {
    return true;
}

bool RatingTable::remove(QSqlQuery &) const {
    return true;
}

bool RatingTable::isValid() const {
    return _data.size();
}

void RatingTable::clear() {
    _data.clear();

    DBObjectQuery::clear();

}

const QMap<short, UserRecord>& RatingTable::data() const {
    return _data;
}

DbAddress RatingTable::getDBAddress() {
    return {table, id};
}

}

