/*
 * Copyright (C) 2018-2021 QuasarApp.
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
 */
class SetSingleValue final: public DBObject
{
public:
    /**
     * @brief SetSingleValue This is default constructor of the update query generator.
     * @param address This is address of the field intended for update object.
     * @param field This is field id (column name ) of the intended for update object.
     * @param value This is a new value.
     */
    SetSingleValue(const DbAddress& address,
                   const QString& field,
                   const QVariant& value);

    DBObject *createDBObject() const override;
    PrepareResult prepareUpdateQuery(QSqlQuery &q) const override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    bool isCached() const override;


protected:
    QString primaryKey() const override;

private:
    QString _field;
    QVariant _value;
};

}
}
#endif // SETSINGLEVALUE_H
