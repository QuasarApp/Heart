/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef IREQUEST_H
#define IREQUEST_H

#include "networkprotocol_global.h"

namespace NP {

/**
 * @brief The Request class - base request methods for data packages
 */
class NETWORKPROTOCOLSHARED_EXPORT Request
{
public:
    Request();
    unsigned char getRequestCmd() const;
    void setRequestCmd(unsigned char value);

protected:
    unsigned char requestCmd = 0;

};
}
#endif // IREQUEST_H
