//#
//# Copyright (C) 2022-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef APIVERSIONPARSER_H
#define APIVERSIONPARSER_H

#include "abstractnodeinfo.h"
#include "iparser.h"

namespace QH {

namespace PKG {
class APIVersion;
class VersionIsReceived;
}
/**
 * @brief The APIVersionParser class This is main parser forthe main command.
 * This parsers work only with the APIVersion packge;
 */
class APIVersionParser: public iParser
{
    Q_OBJECT
public:
    APIVersionParser(AbstractNode* node);

    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              AbstractNodeInfo *sender) override;
    int version() const override;
    QString parserId() const override;

    /**
     * @brief toString This method show all supported commands and them names.
     * @return list of the supported commands as a message
     */
    QString toString() const override;

    /**
     * @brief searchPackage This method search package recursive in all registered pararsers. Searching will be in compatibility versions.
     * Before search methd choose compatibly verson.
     * @param cmd This is command for that shold be create pacakge object.
     * @param ver This is version for that shold be create pacakge object.
     * @param sender This is node that sent @a the cmd.
     * @return Package generated from cmd.
     */
    QSharedPointer<PKG::AbstractData>
    searchPackage(unsigned short cmd,
                  unsigned short ver,
                  AbstractNodeInfo *sender) const;

    /**
     * @brief getSelectedApiParser This method return apiParser for selected node
     * @param apiKey This is key of needed api.
     * @param node This is distanation node.
     * @return needed parser of the distanation node.
     */
    QSharedPointer<iParser> getSelectedApiParser(const QString& apiKey,
                                                 QH::AbstractNodeInfo *node) const;

    /**
     * @brief addApiParser This method add new Api parser for this node.
     * @param parserObject This is bew api parser.
     * @return added parser.
     */
    const QSharedPointer<QH::iParser>&
    addApiParser(const QSharedPointer<QH::iParser>& parserObject);

    /**
     * @brief selectParser This method select api parser betwin nodes.
     * @param distVersion This is information about distanation node version.
     * @return hash map of needed parsers of the distanation node.
     */
    QHash<QString, QSharedPointer<QH::iParser>>
    selectParser(const VersionData& distVersion) const;

    /**
     * @brief parsersTypedCount This method return count of the parsers types.
     * @return count of the parsers types.
     */
    unsigned int parsersTypedCount() const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param cmd this is command that need to parse.
     * @param sender this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(unsigned short cmd,
                                             AbstractNodeInfo *sender) const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param parserKey this is key of the parser type..
     * @param version this is needed version.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(const QString& parserKey,
                                             unsigned short version) const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param parserKey this is key of the parser type..
     * @param sender this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser> selectParser(const QString& parserKey,
                                             AbstractNodeInfo *sender) const;

    /**
     * @brief maximumApiVersion This method return maximum supported api version of this node.
     * @param apiKey This is api key.
     * @return  maximum supported api version of this node.
     */
    unsigned short maximumApiVersion(const QString& apiKey) const;

    /**
     * @brief minimumApiVersion This method return minimum supported api version of this node.
     * @param apiKey This is api key.
     * @return  minimum supported api version of this node.
     */
    unsigned short minimumApiVersion(const QString& apiKey) const;

    /**
     * @brief sendSupportedAPI This method sents all ainformation about suppported api.
     * @param dist This is distanation node.
     * @return true if the information sent successful else false.
     */
    bool sendSupportedAPI(AbstractNodeInfo *dist) const;

    unsigned short selectPackageVersion(const VersionData& local, const VersionData& dist);

signals:

    /**
     * @brief sigNoLongerSupport This signal will be emit when node receive incomplite versions.
     * @param ApiKey This is key of not supported version.
     * @param version This is version
     */
    void sigNoLongerSupport(const QString& ApiKey, unsigned short version);

private:

    QSharedPointer<QH::iParser>
    selectParserImpl(unsigned short cmd, AbstractNodeInfo *sender) const;

    QSharedPointer<QH::iParser>
    selectParserImpl(const QString& key, AbstractNodeInfo *sender) const;

    bool processAppVersion(const QSharedPointer<PKG::APIVersion> &message,
                           AbstractNodeInfo *sender,
                           const QH::Header &);
    bool versionDeliveredSuccessful(const QSharedPointer<PKG::VersionIsReceived> &,
                                    QH::AbstractNodeInfo *sender,
                                    const QH::Header &);

    QHash<QString, QMap<int, QSharedPointer<QH::iParser>>> _apiParsers;

    // This is internal check of registered commands.
    // works only in debug.
    bool commandsValidation(const QSharedPointer<iParser> &parserObject);
};
}
#endif // APIVERSIONPARSER_H
