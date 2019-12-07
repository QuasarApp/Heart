#ifndef DBADDRESS_H
#define DBADDRESS_H

#include "streambase.h"


namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;
    DbAddress(const QString& address, int id);


    QString table;
    int id;

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    friend bool operator== (const DbAddress& left, const DbAddress& other);
};

qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
