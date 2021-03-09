#include "itoken.h"
#include "singlebase.h"

#include <websocket.h>
#include <websocketsubscriptions.h>
namespace QH {

SingleBase::SingleBase()
{

}

bool SingleBase::checkToken(const PKG::AbstractData *pkg) const {
    return dynamic_cast<const IToken*>(pkg);
}

}
