/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBADDRESSKEY_H
#define DBADDRESSKEY_H
#include "abstractkey.h"
#include "dbaddress.h"

namespace QH {

/**
 * @brief The DbAddressKey class is implementation for calc hash key of DbAddress objects.
 */
class HEARTSHARED_EXPORT DbAddressKey: public DbAddress, public AbstractKey
{
public:
    DbAddressKey();
    DbAddressKey(const DbAddress& address);

    /**
     * @brief DbAddressKey This contructor it is vraper of DbAddress::DbAddress constructor.
     * @param table This is name of database table
     * @param value This is value of primary key.
     */
    DbAddressKey(const QString& table, const QVariant& value);

    unsigned int hash() const override;

    bool equal(const AbstractKey *other) const override;

    bool isValid() const override;

    QString toString() const override;

};
}
#endif // DBADDRESSKEY_H
