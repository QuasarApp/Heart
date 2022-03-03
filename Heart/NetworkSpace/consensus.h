/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef CONSENSUS_H
#define CONSENSUS_H

#include "heart_global.h"
#include <QStringList>

namespace QH {

/**
 * @brief The Consensus namespace - a bundle of general rules constants for behavior of nodes network.
 */
namespace Consensus {
    /**
     * @brief Nodes - list of main nodes of network
     */
    extern const QStringList Nodes;

    /**
     * @brief networkPort - sync network port
     */
    extern const short networkPort;

};

}
#endif // CONSENSUS_Hs
