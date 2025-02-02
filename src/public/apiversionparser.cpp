//#
//# Copyright (C) 2022-2025 QuasarApp.
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
        qWarning () << "Can't found requeried parser for versions";
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

QString APIVersionParser::toString() const {
    QString message = " supports parsers:\n";
    message += iParser::toString();

    for (const auto& parser: _apiParsers) {
        for (auto it = parser.begin(); it != parser.end(); ++it) {
            message += "ver: " + QString::number(it.key()) + " " + it.value()->toString();
        }
    }

    return message;
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
            qCritical() << "Internal Error with selection parasers.";
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

    for (const auto &parsersMap : std::as_const(_apiParsers)) {
        for (const auto &parser: parsersMap) {
            if (parser->parserId() == parserObject->parserId()) {
                continue;
            }

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

                qCritical() << err;

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
                                                       AbstractNodeInfo *sender) const{
    auto parser = sender->getParser(cmd);
    if (!parser) {
        parser = selectParserImpl(cmd, sender);
    }

    return parser;
}

QSharedPointer<iParser> APIVersionParser::selectParser(const QString &parserKey,
                                                       AbstractNodeInfo *sender) const {
    if (!sender)
        return nullptr;

    auto parser = sender->getParser(parserKey);
    if (!parser) {
        parser = selectParserImpl(parserKey, sender);
    }

    return parser;
}

QSharedPointer<iParser> APIVersionParser::selectParser(const QString &parserKey,
                                                       unsigned short version) const {

    DistVersion versionData;
    versionData.setMin(version);
    versionData.setMax(version);

    VersionData versions;
    versions.insert(parserKey, versionData);

    return  selectParser(versions).value(parserKey);
}

QSharedPointer<iParser> APIVersionParser::selectParserImpl(unsigned short cmd,
                                                           AbstractNodeInfo *sender) const {
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

QSharedPointer<iParser> APIVersionParser::selectParserImpl(const QString &key,
                                                           AbstractNodeInfo *sender) const{
    auto version = sender->version();
    auto parser = selectParser(version).value(key);
    sender->addParser(parser);

    return parser;
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
    PackagesVersionData multiVersionPackages;

    for (auto it = _apiParsers.begin(); it != _apiParsers.end(); ++it) {
        DistVersion supportVersions;

        for (auto api = it->begin(); api != it->end(); ++api) {
            const auto packages = api.value()->multiVersionPackages();
            for (auto pkg = packages.begin(); pkg != packages.end(); ++pkg) {
                multiVersionPackages[pkg.key()] = pkg.value();
            }
        }

        supportVersions.setMax(it->lastKey());
        supportVersions.setMin(it->firstKey());

        supportedAPIs.insert(it.key(), supportVersions);

    }

    if (supportedAPIs.isEmpty())
        return false;

    PKG::APIVersion versionInformationPkg;
    versionInformationPkg.setApisVersions(supportedAPIs);
    versionInformationPkg.setPackagesVersions(multiVersionPackages);

    return sendData(&versionInformationPkg, dist);
}

bool APIVersionParser::processAppVersion(const QSharedPointer<PKG::APIVersion> &message,
                                         QH::AbstractNodeInfo *sender,
                                         const QH::Header &) {

    auto distVersion = message->apisVersions();
    sender->setVersion(distVersion);
    sender->setMultiVersionPackages(message->packagesVersions());

    auto parser = selectParser(distVersion);

    for (auto parserKey = distVersion.keyBegin(); parserKey != distVersion.keyEnd(); ++parserKey) {
        if (!parser.contains(*parserKey)) {
            auto requiredApi = distVersion.value(*parserKey);

            qCritical() << "Can't found required parser for versions: " << *parserKey << requiredApi.min() << requiredApi.max();

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
