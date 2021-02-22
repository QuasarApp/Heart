/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ITOKEN_H
#define ITOKEN_H

#include "accesstoken.h"

namespace QH {

/**
 * @brief The IToken classis interface for  the validation user packages and user dates.
 */
class IToken
{
public:
    IToken() = default;
    virtual ~IToken() = default;

    /**
     * @brief getSignToken This method should be return actualy token of the user.
     * @return actualy access token.
     */
    virtual const AccessToken& getSignToken() const = 0;
};
}
#endif // ITOKEN_H
