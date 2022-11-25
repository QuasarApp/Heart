//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef APIVERSIONPARSER_H
#define APIVERSIONPARSER_H

#include "abstractnodeinfo.h"
#include "iparser.h"

namespace QH {

class APIVersion;
class VersionIsReceived;

/**
 * @brief The APIVersionParser class This is main parser forthe main command.
 * This parsers work only with the APIVersion packge;
 */
class APIVersionParser: public QObject, public iParser
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
     * @brief searchPackage This method search package recursive in all registered pararsers. Searching will be in compatibility versions.
     * Before search methd choose compatibly verson.
     * @param cmd This is command for that shold be create pacakge object.
     * @param sender This is node that sent @a the cmd.
     * @return Package generated from cmd.
     */
    QSharedPointer<PKG::AbstractData>
    searchPackage(unsigned short cmd, AbstractNodeInfo *sender) const;

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
     */
    void addApiParser(const QSharedPointer<QH::iParser>& parserObject);

    /**
     * @brief addApiParser This is template metod that add sipport of new apiparser @a ApiType
     * @tparam ApiType This is type of new apiParser that will be added to the main parser.
     */
    template<class ApiType, class ... Args >
    void addApiParser(Args&&... arg) {
        addApiParser(QSharedPointer<ApiType>::create(std::forward<Args>(arg)...));
    }

    /**
     * @brief selectParser This method select api parser betwin nodes.
     * @param distVersion This is information about distanation node version.
     * @return hash map of needed parsers of the distanation node.
     */
    QHash<QString, QSharedPointer<QH::iParser>>
    selectParser(const VersionData& distVersion) const;

    /**
     * @brief selectParser This method select parser by command and sender.
     * @param cmd this is command that need to parse.
     * @param sender this is node that sent this command.
     * @return parser for the @a cmd command
     */
    QSharedPointer<QH::iParser>
    selectParser(unsigned short cmd, AbstractNodeInfo *sender);

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

signals:

    /**
     * @brief sigNoLongerSupport This signal will be emit when node receive incomplite versions.
     * @param ApiKey This is key of not supported version.
     * @param version This is version
     */
    void sigNoLongerSupport(const QString& ApiKey, unsigned short version);

private:

    QSharedPointer<QH::iParser>
    selectParserImpl(unsigned short cmd, AbstractNodeInfo *sender);

    bool processAppVersion(const QSharedPointer<APIVersion> &message,
                           AbstractNodeInfo *sender,
                           const QH::Header &);
    bool versionDeliveredSuccessful(const QSharedPointer<VersionIsReceived> &,
                                    QH::AbstractNodeInfo *sender,
                                    const QH::Header &);

    QHash<QString, QMap<int, QSharedPointer<QH::iParser>>> _apiParsers;

};
}
#endif // APIVERSIONPARSER_H
