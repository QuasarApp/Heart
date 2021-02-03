/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "dberrorcodes.h"
namespace QH {

namespace ErrorCodes {

QString DBErrorCodesHelper::toString(unsigned char enumData) {
    switch (enumData) {

    case ErrorCodes::InternalError: {
        return "Internal server error."
               " Please create issue about this problem in the support page "
               " https://github.com/QuasarApp/Heart/issues/new";
    }

    case ErrorCodes::UserExits: {
        return  "Such user already exists ";
    }

    case ErrorCodes::UserNotExits: {
        return "Such user not exists ";
    }

    case ErrorCodes::UserInvalidPasswoed: {
        return "Invalid password ";
    }

    case ErrorCodes::UserAlreadyLogged: {
        return "User Already Logged";
    }

    case ErrorCodes::UserNotLogged: {
        return "User is not Logged";
    }

    case ErrorCodes::TimeOutError: {
        return "Server response timed out ";
    }

    case OperatioForbiden: {
        return "Permission denied";
    }

    default: return AbstractErrorCodesHelper::toString(enumData);

    }
}

}

}
