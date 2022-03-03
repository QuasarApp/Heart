/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef BIGDATAMANAGER_H
#define BIGDATAMANAGER_H

#include <QSharedPointer>
#include <bigdataheader.h>
#include <bigdatapart.h>
#include <bigdatarequest.h>


namespace QH {

class AbstractNode;
class AbstractNodeInfo;


struct PoolData {
    QSharedPointer<PKG::BigDataHeader> header;
    QVector<QSharedPointer<PKG::BigDataPart>> chaindata;
    int lastUpdate = time(0);
};

/**
 * @brief The BigDataManager class is module for control od big data delovering.
 * **How to is work**
 * 1. All big pacakges separate to 64kb parts.
 * 2. Sender send big package header.
 * 3. receiver send request to next paart (0)
 * 4. Sender sent reqested part to receiver
 **/
class BigDataManager
{

public:
    BigDataManager(AbstractNode *);

    /**
     * @brief newPackage This method process first header packge of the big data.
     * @param header This is header package.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if packge processed successful else false
     */
    bool newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                    const AbstractNodeInfo * sender,
                    const Header & pkgHeader);

    /**
     * @brief processPart This method process part of package
     * @param part This is pacakge part.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if packge processed successful else false
     */
    bool processPart(const QSharedPointer<PKG::BigDataPart>& part,
                     const QH::AbstractNodeInfo *sender,
                     const QH::Header &pkgHeader);

    /**
     * @brief finishPart This metho process last package of big data transaction.
     * @param request this is shared pointer to last part of big data transaction.
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return true if pacakge parsed successful else false.
     */
    bool processRequest(const QSharedPointer<PKG::BigDataRequest>& request,
                    const QH::AbstractNodeInfo *sender,
                    const QH::Header &pkgHeader);

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

private:

    void insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header);
    void checkOutDatedPacakges(unsigned int currentProcessedId);

    AbstractNode *_node = nullptr;
    QHash<int, PoolData> _pool;
};
}
#endif // BIGDATAMANAGER_H
