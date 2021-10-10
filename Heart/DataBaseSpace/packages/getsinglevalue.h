/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef GETSINGLEVALUE_H
#define GETSINGLEVALUE_H

#include "dbobject.h"


namespace QH {


namespace PKG {

/**
 * @brief The GetSingleValue class is intended for get a single value from database.
 * The value is selected by primaryKey. By Default the primary key is 'id'
 */
class HEARTSHARED_EXPORT GetSingleValue final: public DBObject
{
    QH_PACKAGE(GetSingleValue, "GetSingleValue")

public:
    /**
     * @brief GetSingleValue This is default constructor of the GetMaxIntegerId class.
     * @param address This is address of getting object.
     * @param field This is name of field.
     * @param primaryKey This is primary key that will be using in selected query.
     */
    GetSingleValue(const DbAddress& address, const QString& field, const QString& primaryKey = "id");

    /**
     * @brief value This method return Maximum value of a sql tables field.
     * @return Maximum value of a sql tables field.
     */
    QVariant value() const;

    DBObject *createDBObject() const override;
    PrepareResult prepareSelectQuery(QSqlQuery &q) const override;
    bool fromSqlRecord(const QSqlRecord &q) override;
    bool isCached() const override;


protected:
    QString primaryKey() const override;

private:
    QString _field;
    QVariant _value;
    QString _key;

};
}
}
#endif // GETSINGLEVALUE_H
