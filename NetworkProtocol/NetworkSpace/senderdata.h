/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SENDERDATA_H
#define SENDERDATA_H
#include "baseid.h"

namespace NP {

/**
 * @brief The SenderData class - this is base interface class for get sender id of data base objects
 */
class NETWORKPROTOCOLSHARED_EXPORT SenderData
{
public:
    SenderData();
    const BaseId& senderID() const;
    void setSenderID(const BaseId &senderID);

protected:
    BaseId _senderID;
};
}
#endif // SENDERDATA_H
