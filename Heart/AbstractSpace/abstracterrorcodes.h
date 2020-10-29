#ifndef AbstractERRORCODES_H
#define AbstractERRORCODES_H

namespace QH {

/**
 * @brief ErrorCodes This namesapce contains all error codes of the Heart Library.
 */
namespace ErrorCodes {

/**
 * @brief The AbstractErrorCodes enum This enum with dafault error codes.
 */
enum AbstractErrorCodes: unsigned char {
    /// This is unknown error. Default value.
    UnknownError = 0,
    /// The sendet to remoute nodes request is invalid. This is default value.
    InvalidRequest,
    /// This case using for inheritance new enum classes.
    AbstractErrorCodes
};
}
}

#endif // ERRORCODES_H
