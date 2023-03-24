/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dbobject.h"
#include <QDataStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QHash>
#include <QSqlRecord>
#include <QVariantMap>
#include <QSharedPointer>
#include <quasarapp.h>
#include <qaglobalutils.h>

namespace QH {
namespace PKG {

DBObject::DBObject() {
    DBObject::clear();
}

DBObject::~DBObject() {

}

PrepareResult DBObject::prepareSelectQuery(QSqlQuery &q) const {

    auto map = variantMap().keys();
    QString queryString = "SELECT " + map.join(",") + " FROM %0 ";
    queryString = queryString.arg(table());


    auto [conditionQueryString, conditionBindingMap] = condition();

    if (conditionQueryString.size()) {

        queryString += " WHERE " + conditionQueryString;
        if (!q.prepare(queryString)) {
            return PrepareResult::Fail;
        }

        for (auto it = conditionBindingMap.begin(); it != conditionBindingMap.end(); ++it) {
            q.bindValue(it.key(), it.value());
        }

        return PrepareResult::Success;
    }


    if (!q.prepare(queryString)) {
        return PrepareResult::Fail;
    }

    return PrepareResult::Success;
}

PrepareResult DBObject::prepareInsertQuery(QSqlQuery &q, bool replace) const {

    DBVariantMap map = variantMap();

    if (!map.size()) {
        QuasarAppUtils::Params::log("The variantMap method return an empty map.",
                                    QuasarAppUtils::Error);

        return PrepareResult::Fail;
    }

    QString queryString = (replace)?
                              "REPLACE INTO %0(%1) VALUES (%2) " :
                              "INSERT INTO %0(%1) VALUES (%2) ";


    queryString = queryString.arg(table());
    QString tableInsertHeader = "";
    QString tableInsertValues = "";

    for (auto it = map.begin(); it != map.end(); ++it) {

        if (!bool(it.value().type & MemberType::Insert)) {
            continue;
        }

        tableInsertHeader += it.key();
        tableInsertValues += ":" + it.key();
        if (std::next(it, 1) != map.end()) {
            tableInsertHeader += ", ";
            tableInsertValues += ", ";
        }

    }

    queryString = queryString.arg(tableInsertHeader);
    queryString = queryString.arg(tableInsertValues);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (!bool(it.value().type & MemberType::Insert)) {
                continue;
            }

            q.bindValue(":" + it.key(), it.value().value);
        }

        return PrepareResult::Success;
    }

    return PrepareResult::Fail;
}

PrepareResult DBObject::prepareUpdateQuery(QSqlQuery &q) const {

    DBVariantMap map = variantMap();

    if (!map.size()) {
        QuasarAppUtils::Params::log("The variantMap method return an empty map.",
                                    QuasarAppUtils::Error);

        return PrepareResult::Fail;
    }

    auto [conditionQueryString, conditionBindingMap] = condition();

    if (conditionQueryString.isEmpty()) {
        QuasarAppUtils::Params::log("The object soue not have condition for update object.",
                                    QuasarAppUtils::Error);
        return PrepareResult::Fail;
    }

    QString queryString = "UPDATE %0 SET %1 WHERE " + conditionQueryString;

    queryString = queryString.arg(table());
    QString tableUpdateValues = "";

    for (auto it = map.begin(); it != map.end(); ++it) {
        if (!bool(it.value().type & MemberType::Update)) {
            continue;
        }

        if (tableUpdateValues.size()) {
            tableUpdateValues += ", ";
        }

        tableUpdateValues += QString("%0= :%0").arg(it.key());

    }

    if (tableUpdateValues.isEmpty()) {
        QuasarAppUtils::Params::log("Fail to generate condition for object: " + toString() +
                                    ". The object do not have valid update fields.",
                                    QuasarAppUtils::Error);
        return PrepareResult::Fail;
    }

    queryString = queryString.arg(tableUpdateValues);

    if (q.prepare(queryString)) {

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (!bool(it.value().type & MemberType::Update)) {
                continue;
            }

            q.bindValue(":" + it.key(), it.value().value);
        }

        for (auto it = conditionBindingMap.begin(); it != conditionBindingMap.end(); ++it) {
#ifdef QT_DEBUG
            if (bool(map.value(it.key()).type & MemberType::Update)) {
                QuasarAppUtils::Params::log(QString("Bad object configuration: "
                                            "The %0 field using in the condition and has MemberType::Update configuration."
                                                    " All condition fields should not use the MemberType::Update. \n %1").
                                            arg(it.key(), toString()),
                                            QuasarAppUtils::Warning);
            }
#endif
            q.bindValue(it.key(), it.value());
        }



        return PrepareResult::Success;
    }

    return PrepareResult::Fail;
}

bool DBObject::isCached() const {
#ifdef HEART_DB_CACHE
    return isHaveAPrimaryKey();
#else
    return false;
#endif
}

bool DBObject::isBundle() const {
    return false;
}

std::pair<QString, QMap<QString, QVariant>> DBObject::condition() const {

    // if object have a primaryKey then return primary key
    auto primaryVal = primaryValue();
    if (!primaryVal.isNull()) {
        return {QString("%0 = :%0").arg(primaryKey()),
                {{QString(":%0").arg(primaryKey()), {primaryVal}}}};
    }


    QuasarAppUtils::Params::log("Fail to generate condition for object: " + toString() +
                                ". Object do not have valid unique fields or valid database address.",
                                QuasarAppUtils::Error);


    return {};
}

QString DBObject::primaryKey() const {
    return "";
}

QVariant DBObject::primaryValue() const {
    return {};
}

bool DBObject::isInsertPrimaryKey() const {
    return bool(variantMap().value(primaryKey()).type & MemberType::Insert);
}

DbAddress DBObject::dbAddress() const {
    return {table(), primaryValue()};
}

QString DBObject::toString() const {
    return AbstractData::toString() +
            QString(" %0").arg(dbAddress().toString());
}

QDataStream &DBObject::fromStream(QDataStream &stream) {
    QuasarAppUtils::Params::log("This object not support stream operator."
                                " Please Override the fromStream method for this object. " + toString(),
                                QuasarAppUtils::Warning);

    return stream;
}

QDataStream &DBObject::toStream(QDataStream &stream) const {

    QuasarAppUtils::Params::log("This object not support stream operator."
                                " Please Override the toStream method for this object. " + toString(),
                                QuasarAppUtils::Warning);
    return stream;
}

PrepareResult DBObject::prepareRemoveQuery(QSqlQuery &q) const {

    QString queryString = "DELETE FROM %0 ";
    queryString = queryString.arg(table());

    auto [conditionQueryString, conditionBindingMap] = condition();

    if (conditionQueryString.size()) {

        queryString += " WHERE " + conditionQueryString;
        if (!q.prepare(queryString)) {
            return PrepareResult::Fail;
        }

        for (auto it = conditionBindingMap.begin(); it != conditionBindingMap.end(); ++it) {
            q.bindValue(it.key(), it.value());
        }

        return PrepareResult::Success;
    }

    QuasarAppUtils::Params::log("This object doues not have condition for remove." + toString(),
                                QuasarAppUtils::Error);

    return PrepareResult::Fail;
}

DBVariantMap DBObject::variantMap() const {
    if (isHaveAPrimaryKey()) {
        return {{primaryKey(), {primaryValue(), MemberType::PrimaryKey}}};
    }

    return {};
}

bool DBObject::isValid() const {
    if (!AbstractData::isValid())
        return false;

    if (isInsertPrimaryKey()) {
        return primaryValue().isValid();
    }

    return table().size();
}

bool DBObject::isHaveAPrimaryKey() const {
    return primaryKey().size();
}

void DBObject::clear() {}

DBVariant::DBVariant() {

}

DBVariant::DBVariant(const QVariant &value, MemberType type) {
    this->value = value;
    this->type = type;
}

}
}
