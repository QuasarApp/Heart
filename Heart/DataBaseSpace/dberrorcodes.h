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
 * @brief The DBErrorCodes enum This is AuthRequest error codes. For more indormation see the QH::AuthRequest class.
 */
enum DBErrorCodes: Code {
    /// User not registered because database not inited or other error occurred.
    InternalError = AbstractErrorCodes::AbstractErrorCodes,
    /// User not have a permision of execute a requested operation.
    OperatioForbiden,
    /// User not registered because user already exists.
    UserExits,
    /// User not logined because you need register user befor login.
    UserNotExits,
    /// User is not Loggined.
    UserNotLogged,
    /// User not logined because have an invalid password.
    UserInvalidPasswoed,
    /// User Already Logged.
    UserAlreadyLogged,
    /// User not logged or registered because timeout. Emitted after 10 sec if not response from server.
    TimeOutError,
    /// This case using for inheritance new enum classes.
    DBErrorCodes,


};

/**
 * @brief The DBErrorCodesHelper class Class for translate the error codes to the string values.
 * This class known about all error codes of the DBErrorCodes enum and AbstractErrorCodes enum.
 */
class DBErrorCodesHelper {
public:
    /**
     * @brief toString This method have a cases for the transate all ErrorCodes of the QuasarApp Heart library.
     * @param enumData This is erorr code.
     * @return string value of the error code.
     * @note Tihis helper sopport all enum from the AbstractErrorCodesHelper class.
     */
    static QString toString(Code enumData);

private:
    DBErrorCodesHelper() = default;

};

}
}
#endif // SingleERRORCODES_H
