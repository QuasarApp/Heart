#ifndef BASENODE_H
#define BASENODE_H

#include "abstractnode.h"

namespace ClientProtocol {

/**
 * @brief The BaseNode class - base inplementation of nodes
 */
class BaseNode : public AbstractNode
{
    Q_OBJECT
public:

    /**
     * @brief BaseNode
     * @param mode
     * @param ptr
     */
    BaseNode(SslMode mode = SslMode::NoSSL, QObject * ptr = nullptr);
    ~BaseNode();
};

}
#endif // BASENODE_H
