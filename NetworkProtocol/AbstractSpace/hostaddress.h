#ifndef HOSTADDRESS_H
#define HOSTADDRESS_H
#include "networkprotocol_global.h"

#include <QHostAddress>
#include "config.h"

namespace NP {

/**
 * @brief The HostAddress class - this is Wraper of QHostAddress
 */
class NETWORKPROTOCOLSHARED_EXPORT HostAddress: public QHostAddress
{
public:
    explicit HostAddress();
    explicit HostAddress(const QHostAddress& other, int port = DEFAULT_PORT);
    explicit HostAddress(const QString& other, int port = DEFAULT_PORT);
    HostAddress(const SpecialAddress& other, int port = DEFAULT_PORT);

    HostAddress(const HostAddress& other);


    unsigned short port() const;
    void setPort(unsigned short port);

    friend QDataStream& operator << (QDataStream& stream, const HostAddress& address);
    friend QDataStream& operator >> (QDataStream& stream, HostAddress& address);

private:
    unsigned short _port = 0;
};

uint qHash(const HostAddress& address);
}

Q_DECLARE_METATYPE(NP::HostAddress);

#endif // HOSTADDRESS_H
