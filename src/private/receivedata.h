/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include "package.h"

namespace QH {

/**
 * @brief The ReceiveData struct This is private structure for contains informaton for parse packages
 */
struct ReceiveData
{
public:
    ReceiveData();

    Package _pkg;
    QByteArray _hdrArray;

};
}
#endif // RECEIVEDATA_H
