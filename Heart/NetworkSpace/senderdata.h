/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SENDERDATA_H
#define SENDERDATA_H
#include "nodeid.h"

namespace QH {

/**
 * @brief The SenderData class - this is base interface class for get sender id of data base objects
 */
class HEARTSHARED_EXPORT SenderData
{
public:
    SenderData();

    const NodeId& senderID() const;
    void setSenderID(const NodeId &senderID);

protected:
    NodeId _senderID;
};
}
#endif // SENDERDATA_H
