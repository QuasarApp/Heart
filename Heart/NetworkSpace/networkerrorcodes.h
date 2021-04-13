#ifndef NETWORKERRORCODES_H
#define NETWORKERRORCODES_H

#include "dberrorcodes.h"

namespace QH {


namespace ErrorCodes {

/**
 * @brief The AbstractEccorCodes enum This enum with dafault error codes.
 */
enum NetworkErrorCodes: unsigned char {
    /// This case using for inheritance new enum classes.
    NetworkErrorCodes = DBErrorCodes::DBErrorCodes,
};
}
}

#endif // NETWORKERRORCODES_H
