#include "itoken.h"
#include "singlebase.h"

#include <websocket.h>
#include <websocketsubscriptions.h>
namespace QH {

SingleBase::SingleBase()
{

}

#define is(X, P) dynamic_cast<X>(P)
bool SingleBase::checkToken(const PKG::AbstractData *pkg) const {
    bool fIgnore = is(const PKG::WebSocket*, pkg) || is(const PKG::WebSocketSubscriptions*, pkg);

    if (fIgnore) {
        return true;
    }

    return dynamic_cast<const IToken*>(pkg);
}
}
