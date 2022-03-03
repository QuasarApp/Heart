/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef AUTHREQUEST_H
#define AUTHREQUEST_H

#include "usermember.h"

#include <request.h>

namespace QH {


namespace PKG {

/**
 * @brief The UserRequestType enum is types of request.
 */
enum class UserRequestType: unsigned char {
    /// Request to registration a new user.
    SignUp,
    /// Request to login an exists user.
    LogIn,
    /// Request to logout an exists user. This request remove the generated accesses token from server.
    LogOut
};

/**
 * @brief The AuthRequest class This class using for send to the SingleServer request to login or registration.
 * Use the Request::getRequestCmd and Request::setRequestCmd methods for set an UserRequestType value.
 */
class HEARTSHARED_EXPORT AuthRequest: public UserMember, public Request
{
    QH_PACKAGE(AuthRequest, "AuthRequest")

public:
    AuthRequest();
    AuthRequest(const Package& pkg);
    unsigned char getRequestCmd() const override;

    // StreamBase interface
    /**
     * @brief setRequest This method set request type for this object.
     * @param request
     */
    void setRequest(const UserRequestType &request);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


private:
    UserRequestType _request;
};
}
}
#endif // AUTHREQUEST_H
