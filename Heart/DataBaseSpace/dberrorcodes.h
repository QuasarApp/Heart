/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DB_ERRORCODES_H
#define DB_ERRORCODES_H

#include "abstracterrorcodes.h"
namespace QH {

namespace ErrorCodes {

/**
 * @brief The DBErrorCodes enum This is AuthRequest error codes. For more information see the QH::AuthRequest class.
 */
enum DBErrorCodes: Code {
    /// User not registered because database not inited or other error occurred.
    InternalError = AbstractErrorCodes::AbstractErrorCodes,
    /// User not have a permission of execute a requested operation.
    OperatioForbiden,
    /// User not registered because user already exists.
    UserExits,
    /// User not logged because you need register user before login.
    UserNotExits,
    /// User is not Logged.
    UserNotLogged,
    /// User not logged because have an invalid password.
    UserInvalidPasswoed,
    /// User Already Logged.
    UserAlreadyLogged,
    /// User not logged or registered because time out. Emitted after 10 sec if not response from server.
    TimeOutError,
    /// This case using for inheritance new enum classes.
    DBErrorCodes,


};

/**
 * @brief The DBErrorCodesHelper class use for translate the error codes to the string values.
 * This class known about all error codes of the DBErrorCodes enum and AbstractErrorCodes enum.
 */
class HEARTSHARED_EXPORT DBErrorCodesHelper {
public:
    /**
     * @brief toString This method have a cases for the translate all ErrorCodes of the QuasarApp Heart library.
     * @param enumData This is error code.
     * @return string value of the error code.
     * @note This helper support all enum from the AbstractErrorCodesHelper class.
     */
    static QString toString(Code enumData);

private:
    DBErrorCodesHelper() = default;

};

}
}
#endif // SingleERRORCODES_H
