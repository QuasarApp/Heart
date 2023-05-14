/*
 * Copyright (C) 2022-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "iparser.h"

#include <abstractdata.h>
#include <qaglobalutils.h>
#include "abstractnode.h"

namespace QH {

iParser::iParser(AbstractNode *parentNode) {
    _node = parentNode;
    debug_assert(_node, "All parsers shold be initialized with parent node");
}

QString iParser::pareseResultToString(const ParserResult &parseResult) {
    switch (parseResult)
    {
    case ParserResult::Processed:
        return "Processed";
    case ParserResult::NotProcessed:
        return "NotProcessed";

    default: return "Error";
    }
}

const PacksMap &iParser::registeredTypes() const {
    return _registeredTypes;
}

QSharedPointer<PKG::AbstractData> iParser::genPackage(unsigned short cmd, unsigned short ver) const {
    return QSharedPointer<PKG::AbstractData>(_registeredTypes.value(cmd, {}).value(ver, [](){return nullptr;})());
}

bool iParser::checkCommand(unsigned short cmd, unsigned short ver) const {
    auto versions = _registeredTypes.value(cmd, {});
    return versions.contains(ver);
}

AbstractNode *iParser::node() const {
    return _node;
}

unsigned int iParser::sendData(const PKG::AbstractData *resp,
                               const HostAddress &address,
                               const Header *req) const{
    return node()->sendData(resp, address, req);
}

unsigned int iParser::sendData(const PKG::AbstractData *resp,
                               const AbstractNodeInfo *dist,
                               const Header *req) const {
    return node()->sendData(resp, dist, req);
}

const PackagesVersionData &iParser::multiVersionPackages() const {
    return _multiVersionPackages;
}

void iParser::initSupportedCommands() {}

QString iParser::toString() const {
    QString message = parserId() + " supports next commands:\n";

    for (const auto& versionsMap: _registeredTypes) {
        for (auto it = versionsMap.keyBegin(); it != versionsMap.keyEnd(); ++it) {
            auto pkg = genPackage(*it);
            message += pkg->cmdString() + ":v" + pkg->ver() + " - " + QString::number(*it) + "\n";
        }
    }

    return message;
}

}
