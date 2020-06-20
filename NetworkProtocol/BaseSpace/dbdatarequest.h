/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBDATAREQUEST_H
#define DBDATAREQUEST_H

#include "dbobject.h"

namespace NP {

/**
 * @brief The DBDataRequest class this is request of db object.
 */
class NETWORKPROTOCOLSHARED_EXPORT DBDataRequest: public DBObject
{
public:
    /**
     * @brief DBDataRequest - creaste empty invalid object
     * @param pkg
     */
    explicit DBDataRequest(const Package &pkg);
    /**
     * @brief DBDataRequest
     * @param address - addrerss of object in database see DbAddress
     */
    explicit DBDataRequest(const DbAddress& address);


    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

};
}
#endif // DBDATAREQUEST_H
