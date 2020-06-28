/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBADDRESS_H
#define DBADDRESS_H

#include "streambase.h"
#include "basedefines.h"
#include "dbid.h"

namespace NP {

/**
 * @brief The DbAddress class - this is address of data base object
 */
class NETWORKPROTOCOLSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;
    DbAddress(const QString& table, const DbId& id);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    friend bool operator== (const DbAddress& left, const DbAddress& other);

    bool isValid() const;

    const QString& table() const;
    void setTable(const QString &table);

    const DbId &id() const;
    void setId(const DbId &id);

private:

    QString _table;
    DbId _id;
};

qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
