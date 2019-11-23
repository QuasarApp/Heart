#ifndef TRANSPORTDATA_H
#define TRANSPORTDATA_H

#include "abstractdata.h"

#include <QHostAddress>


namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT TransportData: public AbstractData
{

public:
    TransportData();
    TransportData(const Package& package);

    const Package& data() const;
    void setData(const Package &data);

    QHostAddress address() const;
    void setAddress(const QHostAddress &address);

    bool isValid() const;

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;
private:
    Package _data;
    QHostAddress _address;


};

}
#endif // TRANSPORTDATA_H
