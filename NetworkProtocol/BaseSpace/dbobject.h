/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "networkprotocol_global.h"
#include "dbaddress.h"
#include "basedefines.h"

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
    virtual DBObject* factory() = 0;

    /**
     * @brief select - override this metod for get item from database
     * @param q
     * @return
     */
    virtual bool select(QSqlQuery& q) = 0;

    /**
     * @brief save - override this method for save item into database
     * @param q
     * @return
     */
    virtual bool save(QSqlQuery& q) const = 0 ;

    /**
     * @brief remove - override this method for remove this item from database.
     * the default implementatin remove item from id or primaryKey
     * @param q
     * @return
     */
    virtual bool remove(QSqlQuery& q) const;

    /**
     * @brief isCached
     * @return return true if item in cache. default implementation retun true only
     */
    virtual bool isCached() const;

    /**
     * @brief dbAddress - unique address of item in database {id:table}
     * @return
     */
    DbAddress dbAddress() const;
protected:
    QString _tableName;
    DbId _id;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


};
}


#endif // DBOBJECT_H
