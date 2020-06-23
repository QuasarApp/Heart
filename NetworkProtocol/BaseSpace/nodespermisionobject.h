/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NODESPERMISIONOBJECT_H
#define NODESPERMISIONOBJECT_H

#include "dbcachekey.h"
#include "dbobject.h"

namespace NP {

/**
 * @brief The NodesPermisionObject class - database object of permisions of node
 */
class NETWORKPROTOCOLSHARED_EXPORT NodesPermisionObject: public DBObject
{
public:
    NodesPermisionObject();
    NodesPermisionObject(const Package& pkg);
    NodesPermisionObject(const QByteArray& id);

    // DBObject interface
    DBObject *factory() override;
    bool select(QSqlQuery &q) override;
    bool save(QSqlQuery &q) const override;

private:
    QHash<DBCacheKey, Permision> _permisions;
};
}
#endif // NODESPERMISIONOBJECT_H
