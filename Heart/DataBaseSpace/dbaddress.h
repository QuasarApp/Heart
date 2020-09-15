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
#include "baseid.h"

namespace QH {

/**
 * @brief The DbAddress class - this is address of data base object
 */
class NETWORKPROTOCOLSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;
    DbAddress(const QString& table, const BaseId& id);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    /**
     * @brief toString - return a string implementation fo this object
     * @return string of object
     */
    QString toString() const;

    friend bool operator== (const DbAddress& left, const DbAddress& other);
    friend bool operator!= (const DbAddress& left, const DbAddress& other);

    bool isValid() const;

    const QString& table() const;
    void setTable(const QString &table);

    const BaseId &id() const;
    void setId(const BaseId &id);

private:

    QString _table;
    BaseId _id;
};

qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
