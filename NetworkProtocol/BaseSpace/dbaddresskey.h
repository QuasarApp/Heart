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

namespace NP {

/**
 * @brief The DbAddressKey class - key provider for DbAddress
 */
class NETWORKPROTOCOLSHARED_EXPORT DbAddressKey: public DbAddress, public AbstractKey
{
public:
    DbAddressKey();
    DbAddressKey(const QString& address, const DbId& id);

    // AbstractKey interface
public:
    unsigned int hash() const override;
};
}
#endif // DBADDRESSKEY_H
