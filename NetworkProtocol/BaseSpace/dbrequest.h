/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBREQUEST_H
#define DBREQUEST_H

#include "dbaddress.h"
#include "request.h"

namespace NP {

/**
 * @brief The DBRequest class is request with dbAddress
 */
class DBRequest : public Request
{
public:
    DBRequest();
    DbAddress address() const;
    void setAddress(const DbAddress &address);

protected:
    DbAddress _address;
};

}
#endif // DBREQUEST_H
