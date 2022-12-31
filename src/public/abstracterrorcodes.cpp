/*
 * Copyright (C) 2020-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstracterrorcodes.h"

namespace QH {

/**
 * @brief ErrorCodes This namesapce contains all error codes of the Heart Library.
 */
namespace ErrorCodes {

QString AbstractErrorCodesHelper::toString(Code enumData) {
    switch (enumData) {
    case NoError: {
        return "Command executed or accepted successful.";
    }

    case InvalidRequest: {
        return "The client node prepare and send invalid request to server. (invalid pacakage received)";
    }

    default: break;
    }

    return "Unknown Error ocurred";
}

}

}

