//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef ABSTRACTNODEPARSER_H
#define ABSTRACTNODEPARSER_H

#include <iparser.h>
#include <ping.h>

namespace QH {

/**
 * @brief The AbstractNodeParser class is main parser of the abstract level of the hear lib.
 */
class AbstractNodeParser: public iParser
{
    Q_OBJECT
public:
    AbstractNodeParser(AbstractNode *parentNode);
    ~AbstractNodeParser() override;
    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              AbstractNodeInfo *sender) override;
    int version() const override;
    QString parserId() const override;

signals:

    /**
     * @brief sigPingReceived This method emited
     * @param ping this is received ping object.
     */
    void sigPingReceived(const QSharedPointer<QH::PKG::Ping> &ping);

};
}
#endif // ABSTRACTNODEPARSER_H
