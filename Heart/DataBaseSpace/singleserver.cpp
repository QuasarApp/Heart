#include "singleserver.h"

#include <user.h>
#include <sqldbcache.h>
#include <basenodeinfo.h>

namespace QH {

SingleServer::SingleServer()
{

}

RegisteruserResult SingleServer::registerNewUser(const PKG::User &user,
                                                 const AbstractNodeInfo *info) {
    if (!db()) {
        return RegisteruserResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (localObject) {
        return RegisteruserResult::UserExits;
    }

    if (!db()->saveObject(&user)) {
        return RegisteruserResult::InternalError;
    };

    return loginUser(user, info);
}

RegisteruserResult SingleServer::loginUser(PKG::User user,
                                           const AbstractNodeInfo *info) {

    if (!db()) {
        return RegisteruserResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (!localObject) {
        return RegisteruserResult::UserNotExits;
    }

    if (localObject->authenticationData() != user.authenticationData()) {
        return RegisteruserResult::UserInvalidPasswoed;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return RegisteruserResult::InternalError;

    AccessToken token((nodeinfo->selfId().toBytes()));

    if (token.isValid()) {
        return RegisteruserResult::UserAlreadyLogged;
    }

    token = localObject->token();

    if (!token.isValid()) {
        token = generateToken(AccessToken::Year);
    }

    auto editableNodeInfo = static_cast<BaseNodeInfo*>(getInfoPtr(info->networkAddress()));
    if (!editableNodeInfo)
        return RegisteruserResult::InternalError;

    editableNodeInfo->setSelfId(token.toBytes());
    user.setToken(token);

    if (!sendData(&user, info->networkAddress())) {
        return RegisteruserResult::InternalError;
    };

    return RegisteruserResult::Success;
}

AccessToken SingleServer::generateToken(int length) {
    return AccessToken(length);
}

}
