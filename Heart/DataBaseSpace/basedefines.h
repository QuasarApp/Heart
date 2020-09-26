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
