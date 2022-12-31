/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SETSINGLEVALUE_H
#define SETSINGLEVALUE_H

#include "dbobject.h"

namespace QH {


namespace PKG {

/**
 * @brief The SetSingleValue class This is simple class - request sets a single value into database.
 * @note This class working direct with the database then you should be use carefully.
 *  Frequent use of this class can negatively affect the speed of your application.
 *
 *  Example of use:
 *
 *  @code{cpp}
 *      ...
 *      bool result = db()->updateObject(SetSingleValue(address, fieldName, newValue));
 *      ...
 *  @endcode
 *
 * @see GetSingleValue
 */
class HEARTSHARED_EXPORT SetSingleValue final: public DBObject
{
    QH_PACKAGE(SetSingleValue, "SetSingleValue")

public:
    /**
     * @brief SetSingleValue This is default constructor of the update query generator.
     * @param address This is address of the field intended for update object.
     * @param field This is field id (column name ) of the intended for update object.
     * @param value This is a new value.
     * @param primaryKey This is primary key that will be used for insert of update value. By Default is is id
     */
    SetSingleValue(const DbAddress& address,
                   const QString& field,
                   const QVariant& value,
                   const QString& primaryKey = "id");

    DBObject *createDBObject() const override;
    PrepareResult prepareUpdateQuery(QSqlQuery &q) const override;
    PrepareResult prepareInsertQuery(QSqlQuery &q, bool replace) const override;

    bool fromSqlRecord(const QSqlRecord &q) override;
    bool isCached() const override;

    QString table() const override;


protected:
    QString primaryKey() const override;
    QString primaryValue() const override;

private:
    QString _id;
    QString _table;
    QString _field;
    QString _primaryKey;
    QVariant _value;
};

}
}
#endif // SETSINGLEVALUE_H
