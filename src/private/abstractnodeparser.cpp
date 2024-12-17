//#
//# Copyright (C) 2022-2024 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "abstractnodeparser.h"
#include "abstractnode.h"
#include "qaglobalutils.h"

#include <badrequest.h>
#include <bigdatarequest.h>
#include <closeconnection.h>
#include <ping.h>

namespace QH {

AbstractNodeParser::AbstractNodeParser(AbstractNode* parentNode): iParser(parentNode) {
    debug_assert(parentNode, "Node object can't be null!");

    registerPackageType<PKG::Ping>();
    registerPackageType<PKG::BadRequest>();
    registerPackageType<PKG::CloseConnection>();
}

AbstractNodeParser::~AbstractNodeParser() {
}

ParserResult AbstractNodeParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                              const Header &pkgHeader,
                                              AbstractNodeInfo *sender) {
    auto nodePtr = node();
    if (!nodePtr) {
        return ParserResult::NotProcessed;
    }

    if (!(sender)) {
        qCritical() << "sender socket is not valid!";
        return ParserResult::Error;
    }

    if (!pkg->isValid()) {
        qCritical() << "incomming package is not valid!";

        nodePtr->changeTrust(sender->networkAddress(), CRITICAL_ERROOR);
        return ParserResult::Error;
    }

    if (PKG::Ping::command() == pkg->cmd()) {
        auto cmd = pkg.staticCast<PKG::Ping>();
        if (!cmd->ansver()) {
            cmd->setAnsver(true);
            nodePtr->sendData(cmd.data(), sender, &pkgHeader);
        }

        emit sigPingReceived(cmd);

        return ParserResult::Processed;
    } else if (PKG::BadRequest::command() == pkg->cmd()) {
        auto cmd = static_cast<PKG::BadRequest *>(pkg.data());

        emit nodePtr->requestError(cmd->errCode(), cmd->err());

        return ParserResult::Processed;

    } else if (PKG::CloseConnection::command() == pkg->cmd()) {
        if (sender->isLocal()) {
            nodePtr->removeNode(sender->networkAddress());
        }
        return ParserResult::Processed;
    }

    return ParserResult::NotProcessed;
}

int AbstractNodeParser::version() const {
    return 1;
}

QString AbstractNodeParser::parserId() const {
    return "HeartLibAbstractAPI";
}
}
