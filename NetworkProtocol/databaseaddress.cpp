#include "databaseaddress.h"
#include <QHash>

namespace NetworkProtocol {

uint qHash(const DataBaseAddress &item){
    return qHash(item.table + QString::number(item.id));
}

}
