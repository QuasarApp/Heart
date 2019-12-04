#include "basenodeinfo.h"
#include "ratingusernode.h"
#include "sqldbcache.h"

#include <badrequest.h>
#include <userdatarequest.h>
#include <quasarapp.h>
#include <ratingtable.h>

namespace NetworkProtocol {

RatingUserNode::RatingUserNode() {

}

ParserResult RatingUserNode::parsePackage(const Package &pkg,
                                    QWeakPointer<AbstractNodeInfo> sender) {

    auto parentResult = BaseNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto strongSender = sender.toStrongRef();

    if (UserDataRequest().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<UserDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (UserData().cmd() == pkg.hdr.command) {
        auto obj = QSharedPointer<UserData>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
        return ParserResult::Processed;

    } else if (RatingTable().cmd() == pkg.hdr.command) {

        auto obj = QSharedPointer<RatingTable>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;

}

QVariantMap RatingUserNode::defaultDbParams() const {
    return BaseNode::defaultDbParams();
}

// bug : user register with id -1 it is all permision to write into all users table.
bool RatingUserNode::registerNewUser(const QWeakPointer<UserDataRequest>& user,
                                       const QHostAddress& address) {
    auto strongUser = user.toStrongRef();

    if (strongUser.isNull()) {
        return false;
    }

    auto node = getInfoPtr(address).toStrongRef().dynamicCast<BaseNodeInfo>();
    if (node.isNull()) {
        return false;
    }

    AccessToken token = AccessToken(AccessToken::Day);

    strongUser->setToken(token);
    node->setToken(token);
    node->setPermision(strongUser->tableName(), strongUser->getId(),
                       Permission::Read | Permission::Write);

    auto _db = db().toStrongRef();

    if (!_db->saveObject(user)) {
        return false;
    }

    return true;
}

bool RatingUserNode::loginUser(const QWeakPointer<UserDataRequest>& user,
                         const QWeakPointer<DBObject>& userdb,
                         const QHostAddress& address) {
    auto strongUser = user.toStrongRef();

    if (strongUser.isNull()) {
        return false;
    }

    auto node = getInfoPtr(address).toStrongRef().dynamicCast<BaseNodeInfo>();
    if (node.isNull()) {
        return false;
    }

    if (node->token().isValid()) {

        if (node->token() == strongUser->token()) {
            node->setPermision(strongUser->tableName(), strongUser->getId(),
                               Permission::Read | Permission::Write);
            return true;
        }
    }

    auto strongUserDB = userdb.toStrongRef().dynamicCast<UserData>();

    if (!strongUserDB.isNull() && strongUserDB->isValid() ) {
        if (strongUserDB->passSHA256() == strongUser->passSHA256()) {
            node->setPermision(strongUser->tableName(), strongUser->getId(),
                               Permission::Read | Permission::Write);
            return true;
        }
    }

    return false;
}

bool RatingUserNode::workWithUserRequest(QWeakPointer<UserDataRequest> rec,
                                           const QHostAddress &addere,
                                           const Header *rHeader) {

    auto request = rec.toStrongRef();

    if (request.isNull())
        return false;

    if (!isListening()) {
        return false;
    }

    auto _db = db().toStrongRef();

    if (_db.isNull()) {
        QuasarAppUtils::Params::verboseLog("Server not inited (db is null)",
                                           QuasarAppUtils::Error);
        return false;
    }

    switch (static_cast<UserDataRequestCmd>(request->getRequestCmd())) {
    case UserDataRequestCmd::Get: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (node->permision(request->tableName(), request->getId()) < Permission::Read) {
            return false;
        }

        auto res = QSharedPointer<UserData>::create().dynamicCast<DBObject>();
        if (!_db->getObject(res)) {
            return false;
        }

        if (!sendData(res, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }
        break;

    }

    case UserDataRequestCmd::Save: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (node->permision(request->tableName(), request->getId()) < Permission::Write) {
            return false;
        }

        if(!_db->saveObject(request)) {
            QuasarAppUtils::Params::verboseLog("do not saved object in database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }

        if (!sendData(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

        // TODO

    case UserDataRequestCmd::Login: {

        auto res = QSharedPointer<UserData>::create().dynamicCast<DBObject>();
        _db->getObject(res);

        if (res->isValid()) {
            // login oldUser
            if (!loginUser(request, res, addere)) {
                return false;
            }
        } else {
            // register a new user;
            if (!registerNewUser(request, addere)) {
                return false;
            }
        }

        if (!sendData(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

    case UserDataRequestCmd::Delete: {

        if(!_db->deleteObject(request)) {
            QuasarAppUtils::Params::verboseLog("do not deleted object from database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }


        if (!sendData(request, addere, rHeader)) {
            QuasarAppUtils::Params::verboseLog("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }
    default: return false;

    }


    return true;

}

}
