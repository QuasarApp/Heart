/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DATABASEADDRESS_H
#define DATABASEADDRESS_H
#include "networkprotocol_global.h"
#include <QString>

namespace NP {

struct NETWORKPROTOCOLSHARED_EXPORT DataBaseAddress
{
    QString table;
    int id;
};

uint qHash(const DataBaseAddress& item);

}
#endif // DATABASEADDRESS_H
