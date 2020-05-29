/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "basenodeinfo.h"
#include "ratingusernode.h"
#include "sqldbcache.h"

#include <badrequest.h>
#include <userdatarequest.h>
#include <quasarapp.h>
#include <ratingtable.h>

namespace NP {

RatingUserNode::RatingUserNode() {

}

ParserResult RatingUserNode::parsePackage(const Package &pkg,
                                    const WP<AbstractNodeInfo> &sender) {

    auto parentResult = BaseNode::parsePackage(pkg, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    auto strongSender = sender.toStrongRef();

    if (H_16<UserDataRequest>() == pkg.hdr.command) {
        auto cmd = SP<UserDataRequest>::create(pkg);

        if (!cmd->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        if (!workWithUserRequest(cmd, strongSender->id(), &pkg.hdr)) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }
        return ParserResult::Processed;


    } else if (H_16<UserData>() == pkg.hdr.command) {
        auto obj = SP<UserData>::create(pkg);
        if (!obj->isValid()) {
            badRequest(strongSender->id(), pkg.hdr);
            return ParserResult::Error;
        }

        emit incomingData(obj, strongSender->id());
        return ParserResult::Processed;

    } else if (H_16<RatingTable>() == pkg.hdr.command) {

        auto obj = SP<RatingTable>::create(pkg);
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
bool RatingUserNode::registerNewUser(const WP<AbstractData>& user,
                                       const QHostAddress& address) {
    auto strongUser = user.toStrongRef().dynamicCast<UserData>();

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

bool RatingUserNode::loginUser(const WP<AbstractData>& user,
                         const WP<AbstractData>& userdb,
                         const QHostAddress& address) {
    auto strongUser = user.toStrongRef().dynamicCast<UserData>();

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

bool RatingUserNode::workWithUserRequest(const WP<AbstractData> &rec,
                                           const QHostAddress &addere,
                                           const Header *rHeader) {

    auto request = rec.toStrongRef().dynamicCast<UserDataRequest>();

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

    switch (static_cast<UserDataRequestCmd>(request->getRequestCmd())) {
    case UserDataRequestCmd::Get: {

        auto node = getInfoPtr(addere).toStrongRef().dynamicCast<BaseNodeInfo>();
        if (node.isNull()) {
            return false;
        }

        if (node->permision(request->tableName(), request->getId()) < Permission::Read) {
            return false;
        }

        auto res = SP<UserData>::create().dynamicCast<DBObject>();
        if (!_db->getObject(res)) {
            return false;
        }

        if (!sendData(res, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
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

        if(!_db->saveObject(rec)) {
            QuasarAppUtils::Params::log("do not saved object in database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }

        if (!sendData(rec, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

        // TODO

    case UserDataRequestCmd::Login: {

        auto res = SP<UserData>::create().dynamicCast<DBObject>();
        res->copyFrom(request.data());

        if (_db->getObject(res)) {
            // login oldUser
            if (!loginUser(rec, res, addere)) {
                return false;
            }
        } else {
            // register a new user;
            if (!registerNewUser(res, addere)) {
                return false;
            }
            _db->getObject(res);
        }

        if (!sendData(res, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
                                               QuasarAppUtils::Warning);
            return false;
        }

        break;
    }

    case UserDataRequestCmd::Delete: {

        if(!_db->deleteObject(rec)) {
            QuasarAppUtils::Params::log("do not deleted object from database!" + addere.toString(),
                                               QuasarAppUtils::Error);
            return false;
        }


        if (!sendData(rec, addere, rHeader)) {
            QuasarAppUtils::Params::log("responce not sendet to" + addere.toString(),
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
