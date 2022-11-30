/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATAPARSER_OLD_H
#define BIGDATAPARSER_OLD_H

#include <iparser.h>


namespace QH {

namespace PKG {
class BigDataHeader;
class BigDataPart;
class BigDataRequest;
class BigDataWraper;
}

class AbstractNode;
class AbstractNodeInfo;

/**
 * @brief The BigDataParserOld class is main manager for control big data packages.
 * @note This is some a  BigDataParser
 */
class BigDataParserOld final: public iParser
{
    struct PoolData {
        QSharedPointer<PKG::BigDataHeader> header;
        QVector<QSharedPointer<PKG::BigDataPart>> chaindata;
        int lastUpdate = time(0);
    };
public:
    template<class T>
    /**
     * @brief registerPackageType This method register package type T.
     * This is need to prepare pacakge for parsing in the parsePackage method.
     */
    void registerPackageTypeOld() {
        _registeredTypes[T::commandOld()] = [](){
            return new T();
        };
    };

    template<class PackageClass,class HandlerType, class HandlerMethod>

    inline ParserResult commandHandlerOld(HandlerType handlerObject, HandlerMethod method,
                                       const QSharedPointer<QH::PKG::AbstractData> &pkg,
                                       QH::AbstractNodeInfo *sender,
                                       const QH::Header &pkgHeader) {

        if (PackageClass::commandOld() == pkg->cmd()) {
            auto data = pkg.staticCast<PackageClass>();

            if (!data->isValid()) {
                return QH::ParserResult::Error;
            }

            if(!(handlerObject->*method)(data, sender, pkgHeader)) {
                return QH::ParserResult::Error;
            }

            return QH::ParserResult::Processed;
        }

        return QH::ParserResult::NotProcessed;
    }

    BigDataParserOld(AbstractNode* parentNode);

    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              AbstractNodeInfo *sender) override;
    int version() const override;
    QString parserId() const override;

protected:

    /**
     * @brief newPackage This method process first header packge of the big data.
     * @param header This is header package.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if packge processed successful else false
     */
    bool newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                    AbstractNodeInfo * sender,
                    const Header & pkgHeader);

    /**
     * @brief processPart This method process part of package
     * @param part This is pacakge part.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if packge processed successful else false
     */
    bool processPart(const QSharedPointer<PKG::BigDataPart>& part,
                     AbstractNodeInfo *sender,
                     const QH::Header &pkgHeader);

    /**
     * @brief finishPart This metho process last package of big data transaction.
     * @param request this is shared pointer to last part of big data transaction.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if pacakge parsed successful else false.
     */
    bool processRequest(const QSharedPointer<PKG::BigDataRequest>& request,
                        QH::AbstractNodeInfo *sender,
                        const QH::Header &pkgHeader);

    /**
     * @brief sendBigDataPackage This method separate big pacakge and sent only heder ot serve.
     * @param data This is package that will be sent to remote node.
     * @param sender This is request object.
     * @param pkgHeader requested header.
     * @return true if package sent successful
     */
    bool processBigDataWraper(const QSharedPointer<PKG::BigDataWraper> &request,
                              AbstractNodeInfo *sender,
                              const Header &pkgHeader);


private:

     /**
     * @brief sendBigDataPackage This method separate big pacakge and sent only heder ot serve.
     * @param data This is package that will be sent to remote node.
     * @param sender This is request object.
     * @param pkgHeader requested header.
     * @return true if package sent successful
     */
     bool sendBigDataPackage(const PKG::AbstractData *data,
                             const QH::AbstractNodeInfo *sender,
                             const Header *pkgHeader);

    void insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header);
    void checkOutDatedPacakges(unsigned int currentProcessedId);

    QHash<int, PoolData> _pool;

};
}
#endif // BIGDATAPARSER_OLD_H
