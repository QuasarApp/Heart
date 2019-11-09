#ifndef DATABASEADDRESS_H
#define DATABASEADDRESS_H
#include "networkprotocol_global.h"
#include <QString>

namespace NetworkProtocol {

struct NETWORKPROTOCOLSHARED_EXPORT DataBaseAddress
{
    QString table;
    int id;
};

uint qHash(const DataBaseAddress& item);

}
#endif // DATABASEADDRESS_H
