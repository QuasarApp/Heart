#include "singleserver.h"

#include <user.h>
#include <sqldbcache.h>

namespace QH {

SingleServer::SingleServer()
{

}

ParserResult SingleServer::parsePackage(const QH::Package &pkg,
                                                const QH::AbstractNodeInfo *sender) {

    auto parentResult = DataBaseNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }



    return ParserResult::NotProcessed;
}

RegisteruserResult SingleServer::registerNewUser(const PKG::User *user) {
    if (!db()) {
        return RegisteruserResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (localObject) {
        return RegisteruserResult::UserExits;
    }

    if (!db()->saveObject(user)) {
        return RegisteruserResult::InternalError;
    };

    return RegisteruserResult::Success;
}

}
