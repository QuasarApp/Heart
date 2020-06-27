/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBDATAREQUEST_H
#define DBDATAREQUEST_H

#include "dbobject.h"

#include <request.h>


namespace NP {

enum class DBRequest  {
    Get,
    Update,
    Set
};

/**
 * @brief The DBDataRequest class this is request of db object.
 */
class NETWORKPROTOCOLSHARED_EXPORT DBDataRequest: public Request {

};
}
#endif // DBDATAREQUEST_H
