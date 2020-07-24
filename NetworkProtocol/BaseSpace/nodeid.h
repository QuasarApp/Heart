/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NODEID_H
#define NODEID_H

#include "baseid.h"
#include <QByteArray>


namespace NP {
/**
 * @brief The NodeId class Wraper For Node Id. need for generate new ids from raw sha256 hashes
 */
class NodeId: public BaseId
{
public:
    NodeId();
    NodeId(const BaseId& other);
    NodeId(const QByteArray& rawHashSha256);
    static bool isNodeId(const QByteArray& raw);

    // BaseId interface
    bool isValid() const;
};
}
#endif // NODEID_H
