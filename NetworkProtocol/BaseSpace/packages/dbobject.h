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
#include "abstractdata.h"
#include "networkprotocol_global.h"
#include "dbaddress.h"
#include "basedefines.h"
#include "dbcachekey.h"
#include "senderdata.h"

class QSqlQuery;

namespace NP {

/**
 * @brief The PrepareResult enum - result of work prepare sql query
 */
enum class PrepareResult {
    /// prepare finished fail.
    Fail,
    /// prepare finished succussful
    Success,
    /// prepare disabled for method. Use this case for disable prepare method for object without errors.
    Disabled,
};

/**
 * @brief The DBObject class- main class for work with data base.
 */
class NETWORKPROTOCOLSHARED_EXPORT DBObject : public AbstractData, public SenderData
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
    BaseId getId() const;

    /**
     * @brief setId - set new id for db object
     * @param id
     */
    void setId(const BaseId& id);

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
     *  the default implementation generate default select: "select * from [table] where id=[id]".
     *  If id is empty this implementation use data from altarnativeKey method.
     * @param q - query object
     * @return true if query is prepared seccussful
     */
    virtual PrepareResult prepareSelectQuery(QSqlQuery& q) const;

    /**
     * @brief fromSqlRecord- this method need to init this object from executed sqlRecord.
     *  default implementation get general dbObject information ( id and table name )
     * @note this method weel be invoke for one object. but if isBundle return 'true' then a function fromSqlRecord moust be invoked foreach all elements of list.
     * @param q - sql record
     * @return true if method finished succesful
     */
    virtual bool fromSqlRecord(const QSqlRecord& q);

    /**
     * @brief prepareSaveQuery - override this method for save item into database
     *  this method need to prepare a query for selected data.
     * @param q - query of requst
     * @return PrepareResult value
     */
    virtual PrepareResult prepareSaveQuery(QSqlQuery& q) const = 0 ;

    /**
     * @brief prepareRemoveQuery - override this method for remove this item from database.
     *  this method need to prepare a query for remove this object.
     *  the default implementatin remove item from id or primaryKey.
     *  If id is empty this implementation use data from altarnativeKey method.
     * @param q - query of requst
     * @return PrepareResult value
     */
    virtual PrepareResult prepareRemoveQuery(QSqlQuery& q) const;

    /**
     * @brief isCached
     * @return return true if item in cache. default implementation retun true only
     */
    virtual bool isCached() const;

    /**
     * @brief isBundle
     *  If thsi function return true then SqlDBWriter create only one object after invoked selectquery.
     *  And if the selectquery function return a list of more 1 elements then a method fromSqlRecord moust be invoked foreach all elements of list.
     * @return true if the object is a selection from a set of database object.
     */
    virtual bool isBundle() const;

    /**
     * @brief dbAddress - unique address of item in database {id:table}
     *  default implementation
     * @return unique key of this object
     */
    virtual uint dbKey() const;

    /**
     * @brief altarnativeKey - this method need to return a altarnative key:value pair for a select object when a object do not have a database id.
     * @default default implementation return empty pair.
     * @return pair of altarnative keys. Key : Value.
     */
    virtual QPair<QString, QString> altarnativeKey() const;

    /**
     * @brief dbAddress
     * @return
     */
    DbAddress dbAddress() const;

protected:
    QString _tableName;
    BaseId _id;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


private:
    QString getWhereBlock() const;
};
}

Q_DECLARE_METATYPE(const NP::DBObject*)
Q_DECLARE_METATYPE(NP::DBObject*)
Q_DECLARE_METATYPE(QList<NP::DBObject *>*);
Q_DECLARE_METATYPE(QList<const NP::DBObject *>*);

#endif // DBOBJECT_H
