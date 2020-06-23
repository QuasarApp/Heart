/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QSqlRecord>
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "networkprotocol_global.h"
#include "dbaddress.h"
#include "basedefines.h"
#include "dbcachekey.h"

class QSqlQuery;

namespace NP {


/**
 * @brief The DBObject class- main class for work with data base.
 */
class NETWORKPROTOCOLSHARED_EXPORT DBObject : public AbstractData
{
public:
    /**
     * @brief DBObject
     */
    DBObject(const QString& tableName);

    ~DBObject() override;

    //// AbstractData interface
    bool isValid() const override;

    /**
     * @brief copyFrom get all data from other
     * @arg other - other data package
     * @return return true if method finished seccussful
     */
    bool copyFrom(const AbstractData * other) override;

    /**
     * @brief getId
     * @return id of objcet
     */
    DbId getId() const;

    /**
     * @brief setId - set new id for db object
     * @param id
     */
    void setId(const DbId& id);

    /**
     * @brief clear
     */
    virtual void clear();

    /**
     * @brief tableName
     * @return
     */
    QString tableName() const;

    /**
     * @brief setTableName set the new table for this objec
     * @param tableName
     */
    void setTableName(const QString &tableName);

    /**
     * @brief factory
     * @return self object pointer
     */
    virtual DBObject* factory() const = 0;

    /**
     * @brief prepareSelectQuery - override this metod for get item from database
     *  this method need to prepare a query for selected data.
     *  the default implementation generate default select: "select * from [table] where id=[id]"
     * @param q - query object
     * @return true if query is prepared seccussful
     */
    virtual bool prepareSelectQuery(QSqlQuery& q) const;

    /**
     * @brief fromSqlRecord- this method need to init this object from executed sqlRecord.
     *  default implementation get general dbObject information ( id and table name )
     * @param q - sql record
     * @return true if method finished succesful
     */
    virtual bool fromSqlRecord(const QSqlRecord& q);

    /**
     * @brief prepareSaveQuery - override this method for save item into database
     *  this method need to prepare a query for selected data.
     * @param q
     * @return
     */
    virtual bool prepareSaveQuery(QSqlQuery& q) const = 0 ;

    /**
     * @brief prepareRemoveQuery - override this method for remove this item from database.
     *  this method need to prepare a query for remove this object.
     *  the default implementatin remove item from id or primaryKey
     * @param q
     * @return
     */
    virtual bool prepareRemoveQuery(QSqlQuery& q) const;

    /**
     * @brief isCached
     * @return return true if item in cache. default implementation retun true only
     */
    virtual bool isCached() const;

    /**
     * @brief dbAddress - unique address of item in database {id:table}
     *  default implementation
     * @return unique key of this object
     */
    virtual DBCacheKey dbKey() const;
protected:
    QString _tableName;
    DbId _id;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


};
}


#endif // DBOBJECT_H
