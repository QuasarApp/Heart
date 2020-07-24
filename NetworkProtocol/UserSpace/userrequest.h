/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef USERDATAREQUEST_H
#define USERDATAREQUEST_H

#include "request.h"
#include "userbasedata.h"


namespace NP {

/**
 * @brief The UserRequestCmd enum - this command for general operation with user (it is delete and registration and login)
 */
enum class UserRequestCmd: unsigned char {
    Invalid,
    Login,
    Delete
};

/**
 * @brief The UserRequest class - this command for work with user (login and delete user from node)
 */
class NETWORKPROTOCOLSHARED_EXPORT UserRequest: public UserBaseData, public Request
{
public:
    UserRequest();
    UserRequest(const Package& package);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    // AbstractData interface
    bool isValid() const;

    // DBObject interface
    void clear();

};

}
#endif // USERDATAREQUEST_H
