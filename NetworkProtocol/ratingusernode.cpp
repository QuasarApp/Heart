#include "basenodeinfo.h"
#include "ratinguserserver.h"
#include "sqldbcache.h"

#include <badrequest.h>
#include <userdatarequest.h>
#include <quasarapp.h>

namespace NetworkProtocol {

RatingUserServer::RatingUserServer() {

}

bool RatingUserServer::parsePackage(const Package &pkg,
                                    QWeakPointer<AbstractNodeInfo> sender) {

    if (!BaseNode::parsePackage(pkg, sender)) {
        return false;
    }

    auto strongSender = sender.toStrongRef();

    if (UserDataRequest().cmd() == pkg.hdr.command) {
        auto cmd = QSharedPointer<UserDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return false;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
        }

    } else if (UserData().cmd() == pkg.hdr.command) {
        auto obj = QSharedPointer<UserData>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
        }

        emit incomingData(obj, strongSender->id());

    }

    return true;

}

QVariantMap RatingUserServer::defaultDbParams() const {
    return BaseNode::defaultDbParams();
}

bool RatingUserServer::registerNewUser(QWeakPointer<UserDataRequest> user,
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

bool RatingUserServer::loginUser(const QWeakPointer<UserDataRequest>& user,
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

bool RatingUserServer::workWithUserRequest(QWeakPointer<UserDataRequest> rec,
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

        QSharedPointer<DBObject> res;
        if (!_db->getObject(request->tableName(), request->getId(), &res)) {
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

        QList<QSharedPointer<DBObject>> res;
        if (!_db->getObjects(request->tableName(), "gmail", request->mail(), res)) {
            return false;
        }

        if (res.isEmpty()) {
            // register a new user;
            if (!registerNewUser(request, addere)) {
                return false;
            }
        } else if (res.size() == 1) {
            // login oldUser
            if (!loginUser(request, res.value(1), addere)) {
                return false;
            }
        } else {
            QuasarAppUtils::Params::verboseLog("user " + request->mail() + " have a clone user",
                                               QuasarAppUtils::Error);

            for (auto obj: res) {
                if (!obj.isNull()) {
                    QuasarAppUtils::Params::verboseLog("delete user " + QString::number(obj->getId()) + " from database",
                                                       QuasarAppUtils::Info);

                    if (!_db->deleteObject(obj->tableName(), obj->getId())) {
                        QuasarAppUtils::Params::verboseLog("delete user " + QString::number(obj->getId()) + " fail",
                                                           QuasarAppUtils::Error);
                    }
                }
            }

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

        if(!_db->deleteObject(request->tableName(), request->getId())) {
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
