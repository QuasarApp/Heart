/*
 * Copyright (C) 2018-2020 QuasarApp.
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
 * @brief The DbAddressKey class - key provider for DbAddress
 */
class HEARTSHARED_EXPORT DbAddressKey: public DbAddress, public AbstractKey
{
public:
    DbAddressKey();
    DbAddressKey(const DbAddress& address);
    DbAddressKey(const QString& address, const BaseId& id);

    unsigned int hash() const override;

    const BaseId &id() const override;
    const QString &table() const override;
    bool equal(const AbstractKey *other) const override;

    bool isValid() const override;

};
}
#endif // DBADDRESSKEY_H
