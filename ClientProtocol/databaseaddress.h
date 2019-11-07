#ifndef DATABASEADDRESS_H
#define DATABASEADDRESS_H
#include "clientprotocol_global.h"
#include <QString>

namespace ClientProtocol {

struct CLIENTPROTOCOLSHARED_EXPORT DataBaseAddress
{
    QString table;
    int id;
};

uint qHash(const DataBaseAddress& item);

}
#endif // DATABASEADDRESS_H
