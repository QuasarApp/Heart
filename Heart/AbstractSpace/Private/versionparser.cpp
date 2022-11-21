/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "versionparser.h"
namespace QH {

VersionParser::VersionParser() {

}

ParserResult VersionParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                         const Header &pkgHeader,
                                         const AbstractNodeInfo *sender) {

//    auto parentResult = AbstractNode::parsePackage(pkg, pkgHeader, sender);
//    if (parentResult != QH::ParserResult::NotProcessed) {
//        return parentResult;
//    }

    // here node must be receive version of connected application.
    // if not use the default version (0)ApplicationVersion
    parentResult = commandHandler<ApplicationVersion>(this, &BaseNode::processAppVersion,
                                                      pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    // here node must be receive responce that version is delivered.
    // when node receive this message then node status are confirmed
    parentResult = commandHandler<VersionIsReceived>(this, &BaseNode::versionDeliveredSuccessful,
                                                     pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    auto distVersion = static_cast<const NodeInfo*>(sender)->version();
    auto parser = selectParser(distVersion);

    if (!parser) {
        QuasarAppUtils::Params::log(QString("Can't found requeried parser for versions: %0-%1").
                                    arg(distVersion.minimum()).
                                    arg(distVersion.maximum()),
                                    QuasarAppUtils::Warning);

        return QH::ParserResult::NotProcessed;
    }

    return parser->parsePackage(pkg, pkgHeader, sender);
}

int VersionParser::version() const {
    return 0;
}

QString VersionParser::parserId() const {
    return "QHNodeVersionParser";
}
}
