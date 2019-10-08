#include "getitem.h"
#include "clientprotocol.h"

namespace ClientProtocol {

GetItem::GetItem()
{
    _class = static_cast<quint8>(Command::GetItem);
}
}
