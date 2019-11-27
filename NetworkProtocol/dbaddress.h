#ifndef DBADDRESS_H
#define DBADDRESS_H

#include "streambase.h"


namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;
    DbAddress(const QString& address, int id);

    friend bool operator== (const DbAddress& left, const DbAddress& other);

    QString table;
    int id;

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;
};

qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
