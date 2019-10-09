#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H

#include "basenodeinfo.h"
#include "clientprotocol.h"

#include <QAbstractSocket>

namespace ClientProtocol {

class AbstractNode
{
private:
    QAbstractSocket *_destination = nullptr;
    QHash<unsigned int, BaseNodeInfo*> _connections;
public:
    AbstractNode();
};
}
#endif // ABSTRACTNODE_H
