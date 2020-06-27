#ifndef BASEDEFINES_H
#define BASEDEFINES_H

namespace NP {

/**
 * @brief The DBOperationResult enum
 */
enum class DBOperationResult {
    /// Node do not know about this operaio
    Unknown,
    /// Node allow this operation and exec it
    Allowed,
    /// Node forbid this operation.
    Forbidden,
};

}
#endif // BASEDEFINES_H
