/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBADDRESS_H
#define DBADDRESS_H

#include "streambase.h"


namespace NP {

/**
 * @brief The DbAddress class - this is address of data base object
 */
class NETWORKPROTOCOLSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;
    DbAddress(const QString& address, int id);


    QString table;
    int id;

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    friend bool operator== (const DbAddress& left, const DbAddress& other);

    bool isValid() const;
};

qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
