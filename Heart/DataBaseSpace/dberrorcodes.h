#ifndef SingleERRORCODES_H
#define SingleERRORCODES_H

#include "abstracterrorcodes.h"
namespace QH {

namespace ErrorCodes {

/**
 * @brief The DBErrorCodes enum This is AuthRequest error codes. For more indormation see the QH::AuthRequest class.
 */
enum DBErrorCodes: unsigned char {
    /// User not registered because database not inited or other error occurred.
    InternalError = AbstractErrorCodes::AbstractErrorCodes,
    /// User not have a permision of execute a requested operation.
    OperatioForbiden,
    /// User not registered because user already exists.
    UserExits,
    /// User not logined because you need register user befor login.
    UserNotExits,
    /// User not logined because have an invalid password.
    UserInvalidPasswoed,
    /// User Already Logged.
    UserAlreadyLogged,
    /// This case using for inheritance new enum classes.
    DBErrorCodes


};
}
}
#endif // SingleERRORCODES_H
