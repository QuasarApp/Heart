/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECTSET_H
#define DBOBJECTSET_H

#include "dbobject.h"


namespace QH {
namespace PKG {

/**
* @brief The DBObjectSet class add features for control database object arrays.
 * You will be able be get or remove multiple objects into database table.
 *
 * For using This class override the DBObject::condition and DBObject::fromSqlRecord methods.
 */
class DBObjectSet: public DBObject
{
public:
    DBObjectSet(const QString table);

    // DBObject interface
public:
    PrepareResult prepareSaveQuery(QSqlQuery &) const override final;
    PrepareResult prepareRemoveQuery(QSqlQuery &q) const override final;
    PrepareResult prepareSelectQuery(QSqlQuery &q) const override final;

    bool isCached() const override final;
    bool isBundle() const override final;

protected:
    BaseId generateId() const override final;
};
}
}
#endif // DBOBJECTSET_H
