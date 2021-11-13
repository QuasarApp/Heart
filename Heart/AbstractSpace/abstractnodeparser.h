#ifndef ABSTRACTNODEPARSER_H
#define ABSTRACTNODEPARSER_H

#include "iparser.h"


namespace QH {

namespace PKG {
    class BigDataHeader;
    class BigDataPart;
    class BigDataRequest;
}

struct PoolData {
    QSharedPointer<PKG::BigDataHeader> header;
    QVector<QSharedPointer<PKG::BigDataPart>> chaindata;
    int lastUpdate = time(0);
};

/**
 * @brief The AbstractNodeParser class This implementation of the iParser interface support next packages:
 * * Ping
 * * BadRequest
 * * CloseConnection
 * * BigDataRequest
 * * BigDataHeader
 * * BigDataPart
 *
 *
 * ### The AbstractNode prarser support all functions of big data transfer.
 *
 *
 * How to work the BigData manager. the BigData manager is module for control od big data delovering.
 * **How to is work**
 * 1. All big pacakges separate to 64kb parts.
 * 2. Sender send big package header.
 * 3. receiver send request to next paart (0)
 * 4. Sender sent reqested part to receiver
 *
 */
class AbstractNodeParser: public IParser
{
public:
    AbstractNodeParser(AbstractNode* node);

    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg, const Header &pkgHeader, const AbstractNodeInfo *sender);

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
protected:
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


private:
    void insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header);
    void checkOutDatedPacakges(unsigned int currentProcessedId);



    AbstractNode *_node = nullptr;
    QHash<int, PoolData> _pool;
};

}

#endif // ABSTRACTNODEPARSER_H
