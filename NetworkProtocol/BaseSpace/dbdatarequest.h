/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBDATAREQUEST_H
#define DBDATAREQUEST_H

#include "dbrequest.h"

#include <abstractdata.h>
#include <QSharedPointer>

namespace NP {

class DBObject;

/**
 * @brief The DBDataRequestCmd enum
 */
enum DBDataRequestCmd : unsigned char {
    /// Invalid data
    Invalid = 0,
    /// Get object from node db
    Get,
    /// save new data to node db
    Set,
    /// delete data from node db
    Delete
};

/**
 * @brief The DBDataRequest class this is request of db object
 */
class NETWORKPROTOCOLSHARED_EXPORT DBDataRequest: public AbstractData, public DBRequest
{
public:
    /**
     * @brief DBDataRequest - creaste empty invalid object
     */
    explicit DBDataRequest();
    /**
     * @brief DBDataRequest
     * @param cmd see DBDataRequestCmd
     * @param address - addrerss of object in database see DbAddress
     */
    explicit DBDataRequest(DBDataRequestCmd cmd, const DbAddress& address);


    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;


    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

};
}
#endif // DBDATAREQUEST_H
