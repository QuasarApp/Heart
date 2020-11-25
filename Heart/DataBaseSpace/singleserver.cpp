#include "singleserver.h"

#include <authrequest.h>
#include <sqldbcache.h>
#include <basenodeinfo.h>
#include <badrequest.h>

namespace QH {

SingleServer::SingleServer()
{

}

RegisteruserResult SingleServer::registerNewUser(PKG::UserMember user,
                                                 const AbstractNodeInfo *info) {
    if (!db()) {
        return RegisteruserResult::InternalError;
    }

    auto localObject = db()->getObject(user);

    if (localObject) {
        return RegisteruserResult::UserExits;
    }

    user.setAuthenticationData(hashgenerator(user.authenticationData()));

    auto requester = getSender(info, &user);

    addUpdatePermission(requester, user.dbAddress(), Permission::Write);

    if (!db()->updateObject(&user)) {
        return RegisteruserResult::InternalError;
    }

    return loginUser(user, info);
}

RegisteruserResult SingleServer::loginUser(PKG::UserMember user,
                                           const AbstractNodeInfo *info) {

    if (!db()) {
        return RegisteruserResult::InternalError;
    }

    auto localObject = db()->getObject(user).value().dynamicCast<const PKG::UserMember>();

    if (localObject.isNull()) {
        return RegisteruserResult::UserNotExits;
    }

    if (localObject->authenticationData() != hashgenerator(user.authenticationData())) {
        return RegisteruserResult::UserInvalidPasswoed;
    }

    auto nodeinfo = dynamic_cast<const BaseNodeInfo*>(info);
    if (!nodeinfo)
        return RegisteruserResult::InternalError;

    AccessToken token((nodeinfo->token().toBytes()));

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

    editableNodeInfo->setToken(token);
    user.setToken(token);
    user.setAuthenticationData("");

    if (!sendData(&user, info->networkAddress())) {
        return RegisteruserResult::InternalError;
    };

    return RegisteruserResult::Success;
}

AccessToken SingleServer::generateToken(int length) {
    return AccessToken(length);
}

ParserResult SingleServer::parsePackage(const Package &pkg, const AbstractNodeInfo *sender) {
    auto parentResult = DataBaseNode::parsePackage(pkg, sender);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<QH::PKG::AuthRequest>() == pkg.hdr.command) {
            QH::PKG::AuthRequest obj(pkg);

            if (!obj.isValid()) {
                badRequest(sender->networkAddress(), pkg.hdr,
                           {
                               ErrorCodes::InvalidRequest,
                               "AuthRequest is invalid"
                           });
                return ParserResult::Error;
            }

            if (!workWithUserRequest(&obj, pkg, sender)) {
                return QH::ParserResult::Error;
            }

            return QH::ParserResult::Processed;

        }

    return QH::ParserResult::NotProcessed;

}

QByteArray SingleServer::hashgenerator(const QByteArray &data) {
    return DataBaseNode::hashgenerator(data + "singelserversoult");
}

bool SingleServer::workWithUserRequest(PKG::UserMember* obj,
                                       const Package &pkg,
                                       const AbstractNodeInfo *sender) {

    RegisteruserResult result = RegisteruserResult::InternalError;

    auto request = dynamic_cast<const Request*>(obj);

    if (request) {
        return false;
    }

    if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::Login)) {
        result = loginUser(*static_cast<PKG::UserMember*>(obj), sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::SignIn)) {
        result = registerNewUser(*obj, sender);
    } else if (request->getRequestCmd() == static_cast<quint8>(PKG::UserRequestType::Remove)) {

        auto requesterId = getSender(sender, obj);

        if (requesterId && deleteObject(requesterId, obj) != DBOperationResult::Allowed) {
            badRequest(sender->networkAddress(), pkg.hdr,
                       {
                           ErrorCodes::OperatioForbiden,
                          " Permission denied"
                       });
        }
    }

    switch (result) {

    case RegisteruserResult::InternalError: {
        QuasarAppUtils::Params::log("Internal error ocured in the loginUser or registerNewUser method.",
                                    QuasarAppUtils::Error);
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::InternalError,
                      "Internal server error."
                      " Please create issue about this problem in the support page "
                      " https://github.com/QuasarApp/Heart/issues/new"
                   },
                   REQUEST_INTERNAL_ERROR);

        return false;
    }

    case RegisteruserResult::UserExits: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::InternalError,
                       "Such user already exists "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case RegisteruserResult::UserNotExits: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserNotExits,
                       "Such user not exists "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case RegisteruserResult::UserInvalidPasswoed: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserInvalidPasswoed,
                       "Invalid password "
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case RegisteruserResult::UserAlreadyLogged: {
        badRequest(sender->networkAddress(), pkg.hdr,
                   {
                       ErrorCodes::UserInvalidPasswoed,
                       "User Already Logged"
                   },
                   REQUEST_LOGIN_ERROR);
        return true;

    }

    case RegisteruserResult::Success: {
        return true;
    }
    }

    badRequest(sender->networkAddress(), pkg.hdr,
               {
                   ErrorCodes::InternalError,
                  "Internal server error."
                  " Please create issue about this problem in the support page "
                  " https://github.com/QuasarApp/Heart/issues/new"
               },
               REQUEST_INTERNAL_ERROR);
    return false;
}

}
