/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BASEDEFINES_H
#define BASEDEFINES_H

namespace QH {

/**
 * @brief The DBOperationResult enum contains the statuses of database operations.
 */
enum class DBOperationResult {
    /// The Node do not know about a permision of operation.
    Unknown,
    /// The Node allow this operation and execute it.
    Allowed,
    /// The Node forbid this operation.
    Forbidden,
};

}
#endif // BASEDEFINES_H
