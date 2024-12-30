/*
 * Copyright (C) 2021-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ITOKEN_H
#define ITOKEN_H

#include "accesstoken.h"

namespace QH {

/**
 * @brief The IToken classes interface for the validation user packages and user dates.
 *
 * This is simple interface for validation packages.
 *
 * Example of use:
 *
 * @code{cpp}
 * class MyPackageClass: public  IToken
 * {
 * ...
 *    const AccessToken& getSignToken() const override;
 * ...
 * }
 * @endcode
 *
 * @warning This is not sign of object. This is just secret token value for validation. This validation method not guaranteed that package save own structure when received to server. If the traffic is not encrypted, a third party can spoof the packet values
 *
 */
class HEARTSHARED_EXPORT IToken
{
public:
    IToken() = default;
    virtual ~IToken() = default;

    /**
     * @brief getSignToken This method should be return actually token of the user.
     * @return actually access token.
     */
    virtual const AccessToken& getSignToken() const = 0;

    /**
     * @brief setSignToken This method sets a new token for object.
     * @param token This is new token value.
     */
    virtual void setSignToken(const AccessToken& token) = 0;

};
}
#endif // ITOKEN_H
