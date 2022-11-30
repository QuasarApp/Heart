//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef ABSTRACTNODEPARSER_OLD_H
#define ABSTRACTNODEPARSER_OLD_H

#include <iparser.h>
#include <ping.h>

namespace QH {

/**
 * @brief The AbstractNodeParserOld class is main parser of the abstract level of the hear lib.
 * @note This class some as AbstractNodeParser
 */
class AbstractNodeParserOld: public iParser
{
    Q_OBJECT
public:
    template<class T>
    /**
     * @brief registerPackageTypeOld This method register package type T.
     * This is need to prepare pacakge for parsing in the parsePackage method.
     */
    void registerPackageTypeOld() {
        _registeredTypes[T::commandOld()] = [](){
            return new T();
        };
    };

    AbstractNodeParserOld(AbstractNode *parentNode);
    ~AbstractNodeParserOld() override;
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
#endif
