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
#include "heart_global.h"
#include "dbaddress.h"
#include "basedefines.h"
#include "dbcachekey.h"

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
     * @brief factory
     * @return clone of self object pointer
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
     *  If this function return true then SqlDBWriter create only one object after invoked selectquery.
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

    /**
     * @brief clone - this nethod create a new object. The new Object is cone of current object.
     * @note If you want to get raw pointer to cone object use a "cloneRaw" method.
     * @return return shared pointer to clone of current object
     */
    QSharedPointer<DBObject> clone() const;

    /**
     * @brief cloneRaw - this method return a raw pointer to clone of this object.
     * @warning - clone object don not removed automatically and may result in a memory leak.
     * @note for get a shared pointer of clone object use the "clone" method.
     * @return retuen raw pointer to cloe of this object.
     */
    DBObject* cloneRaw() const;

    /**
     * @brief toString - return a string implementation fo this object
     * @return string of object
     */
    QString toString() const override;


protected:

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief generateId - override this method for all db Objects.
     * if create id is impasoble ther retrun not valid id.
     * @return retuern Id of database object
     */
    virtual BaseId generateId() const = 0;

    /**
     * @brief init - init this object, prepare work with database.
     * @default - this implementation is create id for object of database.
     *  If method generateId return not valid id this method return false.
     * @return true if object initialized fuccessful else return false.
     */
    bool init() override;

private:
    QString getWhereBlock() const;

    DbAddress _dbId;
};
}

Q_DECLARE_METATYPE(const NP::DBObject*)
Q_DECLARE_METATYPE(NP::DBObject*)
Q_DECLARE_METATYPE(QList<NP::DBObject *>*);
Q_DECLARE_METATYPE(QList<const NP::DBObject *>*);

#endif // DBOBJECT_H
