/*
 * Copyright (C) 2018-2022 QuasarApp.
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
#include "isubscribabledata.h"

class QSqlQuery;

namespace QH {
namespace PKG {

/**
 *  The ONLY_DATABASE_PACKAGE macross is base macros for all database objects that do not use stream and network functions.
*/
#define ONLY_DATABASE_PACKAGE QH_PACKAGE(DBObject, "DBObject")


/**
 * @brief The PrepareResult enum is result of work prepare sql query of dbobjects.
 */
enum class PrepareResult {
    /// prepare is failed.
    Fail,
    /// prepare finished successful.
    Success,
    /// prepare disabled for method. Use this case for disable prepare method for object without errors.
    Disabled,
};

/**
 * @brief The MemberType enum. This enum contains types of members DBObjects classes.
 * for more information see the DBObject::variantMap method.
 */
enum class MemberType {
    /// The Field with this type can not be update and Inserted.
    None = 0x0,

    /// The Field With This type can be inserted but not updated.
    Insert = 0x1,

    /// The Field With This type can be updated but not inserted.
    Update = 0x2,

    /// The Field with this type can not be duplicate on a table. If a Database object do not have a primary key then default implementation for select query try get object by fields with unique type.
    Unique = 0x4,

    /// The Field With This type can be inserted and updated.
    InsertUpdate = Insert | Update,

    /// The primary key field without autoincrement.
    PrimaryKey = Insert | Unique,

    //// The primary key field with autoincrement.
    PrimaryKeyAutoIncrement = Unique
};

constexpr inline uint qHash(MemberType type) {
    return static_cast<uint>(type);
}

/**
 * @brief The DBVariant struct contains QVariant value of the DBObjects member and it type.
*/
struct HEARTSHARED_EXPORT DBVariant {
    DBVariant();

    DBVariant(const QVariant& value, MemberType type);
    QVariant value;
    MemberType type = MemberType::None;
};

/**
 * @brief DBVariantMap this is Map with key, and value with data type.
 * \code{cpp}
 *  QMap<QString key, {QVariant value, MemberType type}>;
 * \endcode
 */
typedef QMap<QString, DBVariant> DBVariantMap;

/**
 * @brief The DBObject class- main class for work with data base.
 * @note If you set in the default constructor primaryKey to empty value. The your object disable cache support. For more information see the DBObject::isCached method.
 *
 * @warning Object with empty table name is invalid.
 * @see DataBaseNode
 */
class HEARTSHARED_EXPORT DBObject : public AbstractData, public ISubscribableData
{
    QH_PACKAGE(DBObject, "DBObject")

public:

    /**
     * @brief DBObject This is default constructor.Before using this class you need set the table name and primary key of this object.
     * @param tableName This is table name.
     */
    DBObject(const QString& tableName);

    ~DBObject() override;

    bool isValid() const override;
    bool copyFrom(const AbstractData * other) override;
    unsigned int subscribeId() const override;

    /**
     * @brief subscribeId This static method convert database address to the subscribe id.
     * @param address This is input value of the database address.
     * @return subscribe id.
     */
    static unsigned int subscribeId(const DbAddress& address);

    /**
     * @brief isHaveAPrimaryKey This method return true if this object has a primary key.
     *  If you want to cache this database object then primary key must be not null.
     * @return true if object has a primary key.
     */
    bool isHaveAPrimaryKey() const;

    /**
     * @brief clear This method clear all data of database object.
     *  Override This method for remove or reset your own members of class.
     *  @note The Default implementation do nothing
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
     * @return pointer of new object with the same type.
     */
    virtual DBObject* createDBObject() const = 0;

    /**
     * @brief prepareSelectQuery This method should be prepare a query for selected data.
     *  Override this method for get item from database.
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
     * @note This method will be invoke for one object. But if isBundle return 'true' then a function fromSqlRecord moust be invoked for each all elements of list.
     * @param q This is query object.
     * @return true if method finished successful.
     *
     * Exampel of override fromSqlRecord method:
     * \code{cpp}
     *  bool ExampleObject::fromSqlRecord(const QSqlRecord &q) {

            id = q.value("id").toInt();
            exampleMember = q.value("exampleMember").toInt();
            return isValid();
        }
     * \endcode
     */
    virtual bool fromSqlRecord(const QSqlRecord& q) = 0;

    /**
     * @brief prepareInsertQuery This method should be prepare a query for insert object into database.
     * You need to create a own insert sql query for this object into database.
     *  Override this method for insert item into database.
     * By Default This method prepare a insert query using the data that returned from the variantMap method.
     *
     * Default insert query have a next template:
     * \code{sql}
     *     INSERT INTO %0(%1) VALUES (%3)
     * \endcode
     *
     *  For more information see the DBObject::variantMap method.
     * @note before creating a own object you need to create a own database structure.
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
     * @note If you want disable this method, just override it and return the PrepareResult::Disabled value.
     */
    virtual PrepareResult prepareInsertQuery(QSqlQuery& q) const;

    /**
     * @brief prepareUpdateQuery this method should be prepare a insert data query.
     *
     * Bt Default This method generate query like this:
     * \code{sql}
     *  UPDATE value SET value = 2 WHERE id = 1
     * \endcode
     *
     * If you want to create a own insert query override this method.
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
     * @brief prepareRemoveQuery This method should be prepare a query for remove this object from a database.
     * Override this method for remove this item from database.
     * The default implementation remove item from id or primaryKey for more information see DBObject::condition method.
     * @param q This is query object.
     * @return PrepareResult object with information about prepare results.
     *
     * @note If you want disable this method just override it and return the PrepareResult::Disabled value.
     */
    virtual PrepareResult prepareRemoveQuery(QSqlQuery& q) const;

    /**
     * @brief isCached This method sholud be return status of object.
     * If this method return true then this object can be saved into cache of database.
     * Override this method and set this value to false for composite objects
     * (objects for which a select query can return several objects
     * or an object that does not have a direct representation in the database
     * but contains common characteristics of several objects).
     * @return True if item in cache.
     * The Default implementation check the primary key if this object and if primary key is valid then return true else return false.
     *
     */
    virtual bool isCached() const;

    /**
     * @brief isBundle This method definef determines whether the object will be abstract (composite objects) or single object
     * If this function return true then SqlDBWriter create only one object after invoked selectquery.
     * And if the selectquery function return a list of more 1 elements then a method fromSqlRecord must be invoked for each all elements of list.
     * @return true if the object is a selection from a set of database object.
     */
    virtual bool isBundle() const;

    /**
     * @brief dbKey This method return unique key for this object.
     * For more information see AbstractKey::hash method.
     * This method calc hash of {id:table} data.
     * @return unique key of this object.
     */
    virtual uint dbKey() const;

    /**
     * @brief dbAddress This method return address of the database object.
     * IF the object is not valid then this method return an invalid database address.
     * @return The database address of current object.
     */
    DbAddress dbAddress() const;

    /**
     * @brief clone This method create a new object. The new Object is clone of current object.
     *  This method is template, so you can set needed type for result.
     * \code{cpp}
     *  MyObject->clone<MyObjectType>();
     * \endcode
     * @note If you want to get raw pointer to the clone object use a DBObject::cloneRaw method.
     * @return The shared pointer to clone of current object.
     */
    template<class Object = DBObject>
    QSharedPointer<Object> clone() const {
        return QSharedPointer<Object>(dynamic_cast<Object*>(cloneRaw()));
    }

    /**
     * @brief cloneRaw This method return a raw pointer to clone of this object.
     * @warning clone object do not removed automatically and may result in a memory leak.
     * @note for get a shared pointer of clone object use the DBObject::clone method.
     * @return The raw pointer to clone of this object.
     */
    DBObject* cloneRaw() const;

    QString toString() const override;

    /**
     * @brief variantMap This method should be create a DBVariantMap implementation of this database object.
     *
     * IF you have the database field with autoincrement you need to remove MemberType::Insert attribute from the your key.
     *  For more information see the MemberType::PrimaryKeyAutoIncrement attribute.
     *
     *
     * Example of return value:
     *
     * \code{cpp}
     *  return {{"name",        {"Andrei",      MemberType::Insert}},
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
     * @note If you want disable this functionality then override this method and return an empty map.
     *  But do not forget override the DBObject::prepareSelectQuery method, because its default implementation return error message.
     */
    virtual DBVariantMap variantMap() const;

    /**
     * @brief printError This method return status of printing error messages for sql quries. by default this propertye is enabled.
     * @return true if printing error messages is enabled else false.
     * @see DBObject::setPrintError
     */
    bool printError() const;

    /**
     * @brief setPrintError This method sets new value for printError propertye.
     * @param newPrintError This is new value for printError propertye
     * @see DBObject::printError
     */
    void setPrintError(bool newPrintError);

protected:

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief condition This method must to return a condition of the WHERE block of the sql query.
     * This method using on default implementation of DBObject::prepareSelectQuery and DBObject::prepareRemoveQuery methods.
     * The default implementation generate when block by map for more information see the variantMap nethod.
     * Override this method for customize your select or delete query.
     * @return condition string.
     * @note This operation can not be block the sql request. Use the QString or int type for values of condition. If you want to  bytes array in condition then override the DBObject::condition method.
     *
     * Example of overriding:
     * \code{cpp}
     *  QString DBObject::condition() const {
            return {"id = '" + getId().toRaw() + "'"};
        }
     * \endcode
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
     * @note If you alredy override the condition method then You can return empty string because this method using in generate default condition only.
     * @see DBObject::condition.
     */
    virtual QString primaryValue() const = 0;

    /**
     * @brief isInsertPrimaryKey This method check primaryKeys type.
     *  If the primary key have a type MemberType::Insert then return true.
     * @return true if the primary key have the MemberType::Insert type.
     */
    bool isInsertPrimaryKey() const;

private:
    QString getWhereBlock() const;
    bool _printError = true;
    QString _table;
};
}
}

Q_DECLARE_METATYPE(const QH::PKG::DBObject*)
Q_DECLARE_METATYPE(QH::PKG::DBObject*)
Q_DECLARE_METATYPE(QList<QH::PKG::DBObject *>*);
Q_DECLARE_METATYPE(QList<const QH::PKG::DBObject *>*);

#endif // DBOBJECT_H
