#ifndef DATABASENODEPARSER_H
#define DATABASENODEPARSER_H

#include <abstractnodeparser.h>



namespace QH {

class DataBaseNode;
namespace PKG {
class WebSocket;
}
/**
 * @brief The DataBaseNodeParser class This parsers support next packages:
 *
 * * WebSocket
 * * WebSocketSubscriptions
 * * DeleteObject
 *
 */
class DataBaseNodeParser: public AbstractNodeParser
{
public:
    DataBaseNodeParser(DataBaseNode* node);

    // IParser interface
public:
    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              const AbstractNodeInfo *sender);
protected:
    bool workWithSubscribe(const PKG::WebSocket &rec,
                           const QVariant &clientOrNodeid,
                           const AbstractNodeInfo *sender);
};

}
#endif // DATABASENODEPARSER_H
