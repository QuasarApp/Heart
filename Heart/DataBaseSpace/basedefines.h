/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BASEDEFINES_H
#define BASEDEFINES_H

#include <QSharedPointer>


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

/**
 * @brief The DBOperationData struct This is result data of the getObjects request.
 */
template <class DATA>
struct DBOperationData {
    /// result value. if isAllowed then the data field must be contains result value.
    DBOperationResult result;
    /// this is result value. If the result field contains Unknown or Forbidden values then a data field contains nullptr value.
    QSharedPointer<DATA> data = nullptr;
};

}
#endif // BASEDEFINES_H
