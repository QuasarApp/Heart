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

namespace QH {
namespace PKG {

/**
 * @brief The PrepareResult enum is result of work prepare sql query of dbobjects.
 */
enum class PrepareResult {
    /// prepare is failed.
    Fail,
    /// prepare finished succussful.
    Success,
    /// prepare disabled for method. Use this case for disable prepare method for object without errors.
    Disabled,
};

/**
 * @brief The MemberType enum. This enum contains types of members DBObjects classes.
 * for more information see the DBObject::variantMap method.
 */
enum class MemberType {
    //// The Field with this type can not be update and Inserted.
    None = 0x0,

    //// The Field With This type can be inserted but not updated.
    Insert = 0x1,

    //// The Field With This type can be updated but not inserted.
    Update = 0x2,

    //// The Field With This type can be inserted and updated.
    InsertUpdate = Insert | Insert
};

constexpr inline uint qHash(MemberType type) {
    return static_cast<uint>(type);
}

/**
 * @brief The DBVariant struct contains QVariant value of the DBObjects member and it type.
 */
struct DBVariant {
    DBVariant();

    DBVariant(const QVariant& value, MemberType type);
    QVariant value;
    MemberType type;
};

/**
 * @brief DBVariantMap this is Map with key and valu with data type.
 * \code{cpp}
 *  QMap<QString key, {QVariant value, MemberType type}>;
 * \endcode
 */
typedef QMap<QString, DBVariant> DBVariantMap;

/**
 * @brief The DBObject class- main class for work with data base.
 * @note If you set in the default constructor primaryKey to empty value. The your object disable cache support. for more information see the DBObject::isCached method.
 *
 * @warning Object witj empty table name is invalid.
 */
class HEARTSHARED_EXPORT DBObject : public AbstractData
{
public:

    /**
     * @brief DBObject This is default constructor.Befor using this class tou need set the table name and primary key of this object.
     * @param tableName This is table name
     */
    DBObject(const QString& tableName);

    ~DBObject() override;

    bool isValid() const override;
    bool copyFrom(const AbstractData * other) override;

    /**
     * @brief isHaveAPrimaryKey This method return true if this object have a primary key.
     *  If you want to cache this database object then primary key must be not null.
     * @return true if object has a primary key.
     */
    bool isHaveAPrimaryKey() const;

    /**
     * @brief getId This method return id of database object. The database id it is pair of an id member of table and a table name.
     * @return The id of database objcet.
     */
    const QVariant &getId() const;

    /**
     * @brief setId This method set new id for current database object.
     * @param id This is new value of id.
     */
    void setId(const QVariant& id);

    /**
     * @brief clear This method clear all data of database object.
     *  Override This method for remove or reset your own members of class.
     */
    virtual void clear();

    /**
     * @brief tableName This method return a table name of the database object.
     * @return string value if the table name.
     */
    QString tableName() const;

    /**
     * @brief createDBObject This method should be create a object with the some type as the object called this method.
     * Example of override:
     * \code{cpp}
     *     return create<MyClassName>();
     * \endcode
     * @note The object created on this method not destroyed automatically.
     * @return pointer of new object with the some type.
     */
    virtual DBObject* createDBObject() const = 0;

    /**
     * @brief prepareSelectQuery This method should be prepare a query for selected data.
     *  Override this metod for get item from database.
     *  The default implementation generate default select:
     *  \code
     *   "select * from [table] where id=[id]".
     *  \endcode
     * This method create where block using the condition method. See DBObject::condition fr more information.
     * @param q This is query object.
     * @return PrepareResult object with information about prepare results.
     *
     * @note If you want disable this mehod just override it and return the PrepareResult::Disabled value.
     */
    virtual PrepareResult prepareSelectQuery(QSqlQuery& q) const;

    /**
     * @brief fromSqlRecord This method should be initialize this object from the executed sqlRecord.
     *  The Default implementation get general dbObject information ( id and table name ).
     *  Override This method for initialize this object from sql query.
     * @note This method weel be invoke for one object. But if isBundle return 'true' then a function fromSqlRecord moust be invoked foreach all elements of list.
     * @param q This is query object.
     * @return true if method finished succesful.
     *
     * Exampel of override fromSqlRecord method:
     * \code{cpp}
     *  bool ExampleObject::fromSqlRecord(const QSqlRecord &q) {
            if (!DBObject::fromSqlRecord(q)) {
                return false;
            }

            exampleMember = q.value("exampleMember").toInt();
            return isValid();
        }
     * \endcode
     */
    virtual bool fromSqlRecord(const QSqlRecord& q);

    /**
     * @brief prepareInsertQuery This method should be prepare a query for insert object into database.
     * You need to create a own insert sql query for this object into database.
     *  Override this metod for insert item into database.
     * By Default This method prepare a insert query using the data that returned from the variantMap method.
     *
     * Default insert query have a next template:
     * \code{sql}
     *     INSERT INTO %0(%1) VALUES (%3)
     * \endcode
     *
     *  For more information see the DBObject::variantMap method.
     * @note befor creating a own object you need to create a own database structure.
     * @param q This is query object.
     * @return PrepareResult object with information about prepare results.
     *
     * Example of overriding:
     * \code{cpp}
     *  PrepareResult ExampleObject::prepareInsertQuery(QSqlQuery &q) const {

            DBVariantMap map = variantMap();


            QString queryString = "INSERT INTO %0(%1) VALUES (%2) ";


            queryString = queryString.arg(tableName());
            QString tableInsertHeader = "";
            QString tableInsertValues = "";

            for (auto it = map.begin(); it != map.end(); ++it) {

                tableInsertHeader += it.key();
                tableInsertValues += ":" + it.key();

                if (it + 1 != map.end()) {
                    tableInsertHeader += ", ";
                    tableInsertValues += ", ";
                }

            }

            queryString = queryString.arg(tableInsertHeader);
            queryString = queryString.arg(tableInsertValues);

            if (q.prepare(queryString)) {

                for (auto it = map.begin(); it != map.end(); ++it) {
                    q.bindValue(":" + it.key(), it.value().value);
                }

                return PrepareResult::Success;
            }

            return PrepareResult::Fail;
        }
     * \endcode
     *
     * @note If you want disable this mehod just override it and return the PrepareResult::Disabled value.
     */
    virtual PrepareResult prepareInsertQuery(QSqlQuery& q) const;

    /**
     * @brief prepareUpdateQuery this method sould be prepare a insert data query.
     *
     * Bt Default This method generate query like this
     * \code{sql}
     *  UPDATE value SET value = 2 WHERE id = 1
     * \endcode
     *
     * IF you want to create a own insert query override this method.
     *
     * Example of overriding this method:
     *
     * \code{cpp}
     *  PrepareResult prepareUpdateQuery(QSqlQuery& q) const {
            DBVariantMap map = variantMap();

            QString queryString = "UPDATE %0 SET %1 WHERE %2";

            queryString = queryString.arg(tableName());
            QString tableUpdateValues = "";
            QString tableUpdateRules = QString("%0 = :%0").
                    arg(primaryKey());

            for (auto it = map.begin(); it != map.end(); ++it) {
                if (!(it.value().type & MemberType::Update)) {
                    continue;
                }

                if (tableUpdateValues.size()) {
                    tableUpdateValues += ", ";
                }

                tableUpdateValues += QString("%0= :%0").arg(it.key());

            }

            queryString = queryString.arg(tableUpdateValues);
            queryString = queryString.arg(tableUpdateRules);

            if (q.prepare(queryString)) {

                for (auto it = map.begin(); it != map.end(); ++it) {
                    if (it.value().type != MemberType::InsertUpdate) {
                        continue;
                    }

                    q.bindValue(":" + it.key(), it.value().value);
                }

                return PrepareResult::Success;
            }

            return PrepareResult::Fail;
        }
     * \endcode
     * @param q This is query object.
     * @return PrepareResult object with information about prepare results.
     */
    virtual PrepareResult prepareUpdateQuery(QSqlQuery& q) const;

    /**
     * @brief prepareRemoveQuery This method method should be prepare a query for remove this object from a database.
     * Override this method for remove this item from database.
     * The default implementatin remove item from id or primaryKey for more information see DBObject::condition method.
     * @param q This is query object.
     * @return PrepareResult object with information about prepare results.
     *
     * @note If you want disable this mehod just override it and return the PrepareResult::Disabled value.
     */
    virtual PrepareResult prepareRemoveQuery(QSqlQuery& q) const;

    /**
     * @brief isCached This method shold be retun status of object.
     * If this method return true then this object can be saved into cache of database.
     * Override this method and set this value to false for composite objects
     * (objects for which a select query can return several objects
     * or an object that does not have a direct representation in the database
     * but contains common characteristics of several objects)
     * @return True if item in cache.
     * The Default implementation check the primary key if this object and if primary key is valid then return true else return false.
     *
     */
    virtual bool isCached() const;

    /**
     * @brief isBundle This method definef determines whether the object will be abstract (composite objects) or singel object
     * If this function return true then SqlDBWriter create only one object after invoked selectquery.
     * And if the selectquery function return a list of more 1 elements then a method fromSqlRecord moust be invoked foreach all elements of list.
     * @return true if the object is a selection from a set of database object.
     */
    virtual bool isBundle() const;

    /**
     * @brief dbKey This method return unique key obje this object.
     * For more information see AbstractKey::hash methid.
     * This method calc hash of {id:table} data.
     * @return unique key of this object
     */
    virtual uint dbKey() const;

    /**
     * @brief dbAddress This method return address of the database object.
     * IF the object is not valid then this method return an invalid database address.
     * @return The database address of current object.
     */
    const DbAddress& dbAddress() const;

    /**
     * @brief clone This method create a new object. The new Object is clone of current object.
     *  This method is template, so you can set needed type for result.
     * \code{cpp}
     *  MyObject->clone<MyObjectType>();
     * \endcode
     * @note If you want to get raw pointer to the clone object use a DBObject::cloneRaw method.
     * @return The Shared pointer to clone of current object.
     */
    template<class Object = DBObject>
    QSharedPointer<Object> clone() const {
        return QSharedPointer<Object>(dynamic_cast<Object*>(cloneRaw()));
    }

    /**
     * @brief cloneRaw This method return a raw pointer to clone of this object.
     * @warning clone object don not removed automatically and may result in a memory leak.
     * @note for get a shared pointer of clone object use the DBObject::clone method.
     * @return The raw pointer to clone of this object.
     */
    DBObject* cloneRaw() const;

    QString toString() const override;

protected:

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief variantMap This method should be create a DBVariantMap implementation of this database object.
     * Example of retuen value:
     *
     * \code{cpp}
     *  return {{"name",        {"Andrei",      MemberType::InsertOnly}},
     *          {"age",         {26,            MemberType::InsertUpdate}},
     *          {"extraData",   {QByteArray{},  MemberType::InsertUpdate}}};
     * \endcode
     *
     * Example of override:
     *
     * \code{cpp}
     *     auto map = ParrentClass::variantMap();
           map.insert("myNewMembers", {_newMember.toBytes(), MemberType::InsertUpdate});
           return map;
     * \endcode
     *
     * @note This method using for create a default sql save request
     * see the DBObject::prepareSaveQuery method for more information.
     * @return the QVariantMap implementation of this database object.
     *
     * @note If you want disable this functionality then override this method and return an empty map. But do not forget override the DBObject::prepareSelectQuery method because its default implementation return error message.
     */
    virtual DBVariantMap variantMap() const;

    /**
     * @brief condition This method must to return a condition of the WHERE block of the sql query.
     * This method using on default implementation of DBObject::prepareSelectQuery and DBObject::prepareRemoveQuery methods.
     * The default implementation return dbId value.
     * \code{cpp}
     *  QString DBObject::condition() const {
            return {"id = '" + getId().toRaw() + "'"};
        }
     * \endcode
     * Override this method for customize your select or delete query.
     * @return condition string.
     */
    virtual QString condition() const;

    /**
     * @brief primaryKey This method must be return the name of primary key of this object table.
     * If you want to create data object without primary key just return empty string.
     * @note If you returned empty value then this method can not be prepare insert update and delete querys.
     * @return The primary key name.
     */
    virtual QString primaryKey() const = 0;

    /**
     * @brief primaryValue This method is wraper of DBAddress::id. If This object do not contains a id value then return invalid value.
     * @return Value of primaryKey ( database id ).
     */
    const QVariant& primaryValue() const;


    /**
     * @brief setDbAddress This method set the new database address.
     * @param This is a new value of database address.
     */
    void setDbAddress(const DbAddress &address);

private:
    QString getWhereBlock() const;

    DbAddress _dbId;
};
}
}

Q_DECLARE_METATYPE(const QH::PKG::DBObject*)
Q_DECLARE_METATYPE(QH::PKG::DBObject*)
Q_DECLARE_METATYPE(QList<QH::PKG::DBObject *>*);
Q_DECLARE_METATYPE(QList<const QH::PKG::DBObject *>*);

#endif // DBOBJECT_H
