#ifndef HOSTADDRESS_H
#define HOSTADDRESS_H
#include "heart_global.h"

#include <QHostAddress>
#include "config.h"

namespace QH {

/**
 * @brief The HostAddress class this is wraper of QHostAddress. Contains the NetworkAddress and network port.
 */
class HEARTSHARED_EXPORT HostAddress: public QHostAddress
{
public:
    explicit HostAddress();
    explicit HostAddress(const QHostAddress& other, int port = DEFAULT_PORT);
    explicit HostAddress(const QString& other, int port = DEFAULT_PORT);
    HostAddress(const SpecialAddress& other, int port = DEFAULT_PORT);

    HostAddress(const HostAddress& other);


    /**
     * @brief port - This method return port of node.
     * @return port of node.
     */
    unsigned short port() const;

    /**
     * @brief setPort This method set new port.
     * @param port This is new value of port.
     */
    void setPort(unsigned short port);

    friend QDataStream& operator << (QDataStream& stream, const HostAddress& address);
    friend QDataStream& operator >> (QDataStream& stream, HostAddress& address);

private:
    unsigned short _port = 0;
};

uint qHash(const HostAddress& address);
}

Q_DECLARE_METATYPE(QH::HostAddress);

#endif // HOSTADDRESS_H
