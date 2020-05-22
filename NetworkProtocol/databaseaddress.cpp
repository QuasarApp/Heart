/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "databaseaddress.h"
#include <QHash>

namespace NP {

uint qHash(const DataBaseAddress &item){
    return qHash(item.table + QString::number(item.id));
}

}
