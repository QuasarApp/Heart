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
#include "permisiondata.h"
#include "permisions.h"

namespace NP {

/**
 * @brief The NodesPermisionObject class - database object of permisions of node
 */
class NETWORKPROTOCOLSHARED_EXPORT NodesPermisionObject: public DBObject
{
public:
    NodesPermisionObject();
    NodesPermisionObject(const Package& pkg);
    NodesPermisionObject(const PermisionData& id);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *other) override;

    // DBObject interface
    PrepareResult prepareSaveQuery(QSqlQuery &q) const override;
    PrepareResult prepareRemoveQuery(QSqlQuery &q) const override;
    PrepareResult prepareSelectQuery(QSqlQuery &q) const override;
    DBObject *factory() const override;
    uint dbKey() const override;

    Permission permisions() const;
    void setPermisions(const Permission &permisions);

    PermisionData key() const;
    void setKey(const PermisionData &key);

protected:
    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    BaseId generateId() const override;

private:
    Permission _permisions;
    PermisionData _key;


};
}
#endif // NODESPERMISIONOBJECT_H
