//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "apiversionparser.h"
#include "abstractnodeinfo.h"
#include "distversion.h"

#include <apiversion.h>
#include <versionisreceived.h>
#include <quasarapp.h>
#include <versionisreceived.h>
#include <abstractnode.h>
#include <qaglobalutils.h>
#include <apiversion.h>

namespace QH {

APIVersionParser::APIVersionParser(AbstractNode *node): iParser(node) {

}

ParserResult APIVersionParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                            const Header &pkgHeader,
                                            AbstractNodeInfo *sender) {

    // here node must be receive version of connected application.
    // if not use the default version (0)APIVersion
    auto parentResult = commandHandler<PKG::APIVersion>(this, &APIVersionParser::processAppVersion,
                                                   pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    // here node must be receive responce that version is delivered.
    // when node receive this message then node status are confirmed
    parentResult = commandHandler<PKG::VersionIsReceived>(this, &APIVersionParser::versionDeliveredSuccessful,
                                                     pkg, sender, pkgHeader);
    if (parentResult != QH::ParserResult::NotProcessed) {
        return parentResult;
    }

    const auto parser = selectParser(pkg->cmd(), sender);

    if (!parser) {
        QuasarAppUtils::Params::log(QString("Can't found requeried parser for versions"),
                                    QuasarAppUtils::Warning);
        return ParserResult::NotProcessed;
    }

    auto perserResult = parser->parsePackage(pkg, pkgHeader, sender);
    if (perserResult != QH::ParserResult::NotProcessed) {
        return perserResult;
    }

    return ParserResult::NotProcessed;
}

int APIVersionParser::version() const {
    return 0;
}

QString APIVersionParser::parserId() const {
    return "APIVersionParser";
}

QSharedPointer<PKG::AbstractData>
APIVersionParser::searchPackage(unsigned short cmd,
                                AbstractNodeInfo *sender) const {
    if (!sender)
        return nullptr;

    if (cmd == PKG::APIVersion::command()) {
        return QSharedPointer<PKG::APIVersion>::create();
    } else if (cmd == PKG::VersionIsReceived::command()) {
        return QSharedPointer<PKG::VersionIsReceived>::create();
    }

    auto distVersion = sender->version();
    const auto parsers = selectParser(distVersion);

    if (parsers.isEmpty()) {
        return nullptr;
    }

    for (const auto& parser: parsers) {
        if (!parser) {
            QuasarAppUtils::Params::log(QString("Internal Error with selection parasers."),
                                        QuasarAppUtils::Error);
            continue;
        }

        if (auto package = parser->genPackage(cmd))
            return package;
    }

    return nullptr;
}

QSharedPointer<iParser>
APIVersionParser::getSelectedApiParser(const QString &apiKey,
                                       AbstractNodeInfo *node) const {
    return selectParser(node->version()).value(apiKey, nullptr);
}

bool QH::APIVersionParser::commandsValidation(const QSharedPointer<iParser> &parserObject) {
#ifdef QT_DEBUG
    auto types = QSet<unsigned short>{parserObject->registeredTypes().keyBegin(),
                                      parserObject->registeredTypes().keyEnd()};
    int typesSize = types.size();

    for (const auto &parsersMap : qAsConst(_apiParsers)) {
        for (const auto &parser: parsersMap) {
            auto localTypes = QSet<unsigned short>{parser->registeredTypes().keyBegin(),
                                                   parser->registeredTypes().keyEnd()};
            types -= localTypes;

            if (types.size() != typesSize) {
                auto err = QString("Parser object can't be added. "
                                   "Because commands of the %0_v%1 parser "
                                   "already registered in the %2_v%3 parser. "
                                   "Use the iParser::initSupportedCommands method to "
                                   "fix this issue. "
                                   "See our documentation for get more inforamtion. "
                                   "https://quasarapp.ddns.net:3031/docs/QuasarApp/Heart/latest/classQH_1_1iParser.html").
                           arg(parserObject->parserId()).
                           arg(parserObject->version()).
                           arg(parser->parserId()).
                           arg(parser->version());

                QuasarAppUtils::Params::log(err, QuasarAppUtils::Error);

                return false;
            }
        }
    }
#endif

    return true;
}

const QSharedPointer<QH::iParser>& APIVersionParser::addApiParser(const QSharedPointer<iParser> &parserObject) {
    debug_assert(parserObject, "Parser object should not be nullptr");

    parserObject->initSupportedCommands();

    if (!commandsValidation(parserObject)) {
        debug_assert(false, "Parser object can't be added. "
                            "Because its commands already registered "
                            "in the another parsers.");
    }

    _apiParsers[parserObject->parserId()][parserObject->version()] = parserObject;
    return _apiParsers[parserObject->parserId()][parserObject->version()];
}

QHash<QString, QSharedPointer<iParser>>
APIVersionParser::selectParser(const VersionData &distVersion) const {
    QHash<QString, QSharedPointer<iParser>> result;

    for (auto it = distVersion.begin(); it != distVersion.end(); ++it) {
        for (int version = it->max(); version >= it->min(); --version) {
            auto parser = _apiParsers[it.key()].value(version, nullptr);
            if (parser) {
                result[it.key()] = parser;
                break;
            }
        }
    }

    return result;
}

unsigned int APIVersionParser::parsersTypedCount() const {
    return _apiParsers.size();
}

QSharedPointer<iParser> APIVersionParser::selectParser(unsigned short cmd,
                                                       AbstractNodeInfo *sender) {
    auto parser = sender->getParser(cmd);
    if (!parser) {
        parser = selectParserImpl(cmd, sender);
    }

    return parser;
}

QSharedPointer<iParser> APIVersionParser::selectParserImpl(unsigned short cmd,
                                                           AbstractNodeInfo *sender) {
    auto version = sender->version();
    const auto availableParser = selectParser(version);
    for (const auto& parser: availableParser) {
        if (parser->checkCommand(cmd)) {
            sender->addParser(cmd, parser);
            return parser;
        }
    }

    return nullptr;
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

bool APIVersionParser::sendSupportedAPI(AbstractNodeInfo *dist) const {
    VersionData supportedAPIs;

    for (auto it = _apiParsers.begin(); it != _apiParsers.end(); ++it) {
        DistVersion supportVersions;

        supportVersions.setMax(it->lastKey());
        supportVersions.setMin(it->firstKey());

        supportedAPIs.insert(it.key(), supportVersions);

    }

    if (supportedAPIs.isEmpty())
        return false;

    PKG::APIVersion versionInformationPkg;
    versionInformationPkg.setVersion(supportedAPIs);

    return sendData(&versionInformationPkg, dist);
}

bool APIVersionParser::processAppVersion(const QSharedPointer<PKG::APIVersion> &message,
                                         QH::AbstractNodeInfo *sender,
                                         const QH::Header &) {

    auto distVersion = message->version();
    sender->setVersion(distVersion);
    auto parser = selectParser(distVersion);

    for (auto parserKey = distVersion.keyBegin(); parserKey != distVersion.keyEnd(); ++parserKey) {
        if (!parser.contains(*parserKey)) {
            auto requiredApi = distVersion.value(*parserKey);

            QuasarAppUtils::Params::log(QString("Can't found %0 parser for versions: %1-%2").
                                        arg(*parserKey).
                                        arg(requiredApi.min()).
                                        arg(requiredApi.max()),
                                        QuasarAppUtils::Error);

            unsigned short distMinVersion = sender->version().value(*parserKey).min();

            if (distMinVersion > maximumApiVersion(*parserKey)) {
                emit sigNoLongerSupport(*parserKey, distMinVersion);
            }

            return false;
        }
    }

    sender->setFVersionReceived(true);

    PKG::VersionIsReceived result;
    return node()->sendData(&result, sender);
}

bool APIVersionParser::versionDeliveredSuccessful(const QSharedPointer<PKG::VersionIsReceived> &,
                                                  AbstractNodeInfo *sender,
                                                  const QH::Header &) {
    sender->setFVersionDelivered(true);
    return true;
}

}
