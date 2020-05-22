/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef USERDATAREQUEST_H
#define USERDATAREQUEST_H

#include "request.h"
#include "userdata.h"


namespace NP {

enum class UserDataRequestCmd: unsigned char {
    Invalid,
    Get,
    Login,
    Save,
    Delete
};

class NETWORKPROTOCOLSHARED_EXPORT UserDataRequest: public UserData, public Request
{
public:
    UserDataRequest();
    UserDataRequest(const Package& package);

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
