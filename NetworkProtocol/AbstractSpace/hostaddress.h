#ifndef HOSTADDRESS_H
#define HOSTADDRESS_H
#include "networkprotocol_global.h"

#include <QHostAddress>

namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT HostAddress: public QHostAddress
{
public:
    explicit HostAddress();
    explicit HostAddress(const QHostAddress& other, int port);


    unsigned short port() const;
    void setPort(unsigned short port);

private:
    unsigned short _port = 0;
};
}
#endif // HOSTADDRESS_H
