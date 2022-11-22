//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "apiversionparser.h"
#include "abstractnodeinfo.h"

#include <apiversion.h>
#include <versionisreceived.h>
#include <quasarapp.h>
#include <versionisreceived.h>

namespace QH {

APIVersionParser::APIVersionParser() {

}

ParserResult APIVersionParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                            const Header &pkgHeader,
                                            AbstractNodeInfo *sender) {

    // here node must be receive version of connected application.
    // if not use the default version (0)APIVersion
    auto parentResult = commandHandler<APIVersion>(this, &APIVersionParser::processAppVersion,
                                                   pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    // here node must be receive responce that version is delivered.
    // when node receive this message then node status are confirmed
    parentResult = commandHandler<VersionIsReceived>(this, &APIVersionParser::versionDeliveredSuccessful,
                                                     pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    auto distVersion = sender->version();
    const auto parsers = selectParser(distVersion);

    if (parsers.isEmpty()) {
        QuasarAppUtils::Params::log(QString("Can't found requeried parser for versions"),
                                    QuasarAppUtils::Warning);
        return ParserResult::NotProcessed;
    }

    for (const auto& parser: parsers) {
        if (!parser) {
            QuasarAppUtils::Params::log(QString("Internal Error with selection parasers."),
                                        QuasarAppUtils::Error);
            continue;
        }

        auto perserResult = parser->parsePackage(pkg, pkgHeader, sender);
        if (perserResult != QH::ParserResult::NotProcessed) {
            return perserResult;
        }
    }

    return ParserResult::NotProcessed;
}

int APIVersionParser::version() const {
    return 0;
}

QString APIVersionParser::parserId() const {
    return "APIVersionParser";
}

QSharedPointer<iParser>
APIVersionParser::getSelectedApiParser(const QString &apiKey,
                                       AbstractNodeInfo *node) const {
    return selectParser(node->version()).value(apiKey, nullptr);
}

void APIVersionParser::addApiParser(const QSharedPointer<iParser> &parserObject) {
    _apiParsers[parserObject->parserId()][parserObject->version()] = parserObject;
}

QHash<QString, QSharedPointer<iParser>>
APIVersionParser::selectParser(const VersionData &distVersion) const {
    QHash<QString, QSharedPointer<iParser>> result;

    for (auto it = distVersion.begin(); it != distVersion.end(); ++it) {
        for (int version = it->max; version >= it->min; --version) {
            auto parser = _apiParsers[it.key()].value(version, nullptr);
            if (parser)
                result[it.key()] = parser;
        }
    }

    return result;
}

unsigned short APIVersionParser::maximumApiVersion(const QString &apiKey) const {

    auto availableVersion = _apiParsers.value(apiKey, {});
    if (availableVersion.size()) {
        return availableVersion.lastKey();
    }

    return 0;
}

unsigned short APIVersionParser::minimumApiVersion(const QString &apiKey) const {
    auto availableVersion = _apiParsers.value(apiKey, {});
    if (availableVersion.size()) {
        return availableVersion.firstKey();
    }

    return 0;
}

bool APIVersionParser::processAppVersion(const QSharedPointer<APIVersion> &message,
                                         QH::AbstractNodeInfo *sender,
                                         const QH::Header &) {

    auto distVersion = message->version();
    sender->setVersion(distVersion);
    auto parser = selectParser(distVersion);

    for (const auto &parserKey: message->version().keys()) {
        if (!parser.contains(parserKey)) {
            auto requiredApi = distVersion.value(parserKey);

            QuasarAppUtils::Params::log(QString("Can't found %0 parser for versions: %1-%2").
                                        arg(parserKey).
                                        arg(requiredApi.min).
                                        arg(requiredApi.max),
                                        QuasarAppUtils::Error);

            unsigned short distMinVersion = sender->version().value(parserKey).min;

            if (distMinVersion > maximumApiVersion(parserKey)) {
                emit sigNoLongerSupport(parserKey, distMinVersion);
            }

            return false;
        }
    }

    VersionIsReceived result;
    return node().send(&result, sender);
}

bool APIVersionParser::versionDeliveredSuccessful(const QSharedPointer<VersionIsReceived> &,
                                                  AbstractNodeInfo *sender,
                                                  const QH::Header &) {
    sender->setFVersionDelivered(true);
    return true;
}

}
