/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NODESPERMISIONOBJECT_H
#define NODESPERMISIONOBJECT_H

#include "dbcachekey.h"
#include "dbobject.h"
#include "permisiondata.h"
#include "permission.h"

namespace QH {
namespace PKG {

/**
 * @brief The NodesPermisionObject class contains permisions data of node or client.
 * This class is representation of database object from the MemberPermisions table.
 */
class HEARTSHARED_EXPORT MemberPermisionObject: public DBObject
{
public:
    MemberPermisionObject();
    MemberPermisionObject(const Package& pkg);
    MemberPermisionObject(const PermisionData& id);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *other) override;

    // DBObject interface
    DBObject *createDBObject() const override;
    uint dbKey() const override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    bool isCached() const override;
    DBVariantMap variantMap() const override;

    /**
     * @brief permisions This method return permision of object.
     * For select object set it id using a MemberPermisionObject::setKey method.
     * @return permision level. For more information see the Permission enum.
     */
    Permission permisions() const;

    /**
     * @brief setPermisions This method set a new value of permisions level.
     * @param permisions This is a new value of permisions.
     */
    void setPermisions(const Permission &permisions);

    /**
     * @brief key This method return key (database address) of a checked object.
     * @return PermisionData key. For more information see PermisionData class.
     */
    PermisionData key() const;

    /**
     * @brief setKey This method set new value of PermisionData.
     * @param key This is new value of PermisionData.
     */
    void setKey(const PermisionData &key);

protected:
    /**
     * @brief MemberPermisionObject This is protected constructor for support inheritance functions.
     */
    MemberPermisionObject(const QString& tableName);


    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    QString condition() const override;
    QString primaryKey() const override;
    bool init() override;

private:
    Permission _permision;
    PermisionData _key;

};
}
}
#endif // NODESPERMISIONOBJECT_H
