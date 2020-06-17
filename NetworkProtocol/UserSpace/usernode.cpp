#include "usernode.h"
#include <quasarapp.h>
#include <userrequest.h>
#include <badrequest.h>
#include "sqldbcache.h"
#include "usernodeinfo.h"

namespace NP {

UserNode::UserNode()
{

}

ParserResult UserNode::parsePackage(const Package &pkg,
                                    const AbstractNodeInfo* sender) {

    auto parentResult = BaseNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (H_16<UserRequest>() == pkg.hdr.command) {
        auto cmd = UserRequest(pkg);

        if (!cmd->isValid()) {
            badRequest(sender->networkAddress(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<UserBaseData>() == pkg.hdr.command)  {
        auto cmd = SP<UserBaseData>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }



        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;

}

bool UserNode::workWithUserRequest(const SP<UserRequest> &request,
                                   const QHostAddress &addere,
                                   const Header *rHeader) {

    if (request.isNull())
        return false;

    if (!isListening()) {
        return false;
    }

    auto _db = db().toStrongRef();

    if (_db.isNull()) {
        QuasarAppUtils::Params::log("Server not inited (db is null)",
                                    QuasarAppUtils::Error);
        return false;
    }

    switch (static_cast<UserRequestCmd>(request->getRequestCmd())) {

    case UserRequestCmd::Login: {

        auto res = SP<UserBaseData>::create().dynamicCast<DBObject>();
        res->copyFrom(request.data());

        if (getObject(res, addere, res->dbAddress())) {
            // login oldUser
            if (!loginUser(request, res, addere)) {
                return false;
            }
        } else {
            // register a new user;
            if (!registerNewUser(res, addere)) {
                return false;
            }

            getObject(res, addere, res->dbAddress());
        }

        if (!sendData(res, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
                                        QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

    case UserRequestCmd::Delete: {

        if(!_db->deleteObject(request)) {
            QuasarAppUtils::Params::log("do not deleted object from database!" + addere.toString(),
                                        QuasarAppUtils::Error);
            return false;
        }

        break;
    }
    default: return false;

    }


    return true;

}


bool UserNode::loginUser(const WP<AbstractData>& user,
                         const WP<AbstractData>& userdb,
                         const QHostAddress& address) {
    auto strongUser = user.toStrongRef().dynamicCast<UserBaseData>();

    if (strongUser.isNull()) {
        return false;
    }

    auto strongUserDB = userdb.toStrongRef().dynamicCast<UserBaseData>();

    auto node = getInfoPtr(address).toStrongRef().dynamicCast<UserNodeInfo>();
    if (node.isNull()) {
        return false;
    }

    if (strongUserDB->token().isValid() &&
            strongUser->token() == strongUserDB->token()) {
        node->setUserId(strongUserDB->getId());
        return true;
    }

    if (!strongUserDB.isNull() && strongUserDB->isValid() ) {
        if (strongUserDB->passSHA256() == strongUser->passSHA256()) {
            node->setUserId(strongUserDB->getId());
            return true;
        }
    }

    return false;
}

// bug : user register with id -1 it is all permision to write into all users table.
bool UserNode::registerNewUser(const WP<AbstractData>& user,
                               const QHostAddress& address,
                               bool rememberMe) {
    auto strongUser = user.toStrongRef().dynamicCast<UserBaseData>();

    if (strongUser.isNull()) {
        return false;
    }

    auto node = getInfoPtr(address).toStrongRef().dynamicCast<UserNodeInfo>();
    if (node.isNull()) {
        return false;
    }

    AccessToken token = AccessToken((rememberMe)? AccessToken::Month : AccessToken::Day);
    strongUser->setToken(token);

    auto _db = db().toStrongRef();

    if (!_db->saveObject(user)) {
        return false;
    }

    node->setUserId(strongUser->getId());

    return true;
}

QVariantMap UserNode::defaultDbParams() const {
    return BaseNode::defaultDbParams();
}




}
