#include "itoken.h"
#include "singlebase.h"

#include <abstractnetworkmember.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include "usermember.h"

namespace QH {

SingleBase::SingleBase() {
    registerPackageType<PKG::UserMember>();

}

bool SingleBase::checkToken(const PKG::AbstractData *pkg) const {
    return dynamic_cast<const IToken*>(pkg);
}

}
