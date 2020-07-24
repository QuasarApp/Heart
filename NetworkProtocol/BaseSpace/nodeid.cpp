#include "nodeid.h"
#include "quasarapp.h"

#define NODE_ID 'N'
namespace NP {


NodeId::NodeId() = default;

NodeId::NodeId(const BaseId &other):BaseId(other) {}

NodeId::NodeId(const QByteArray &rawHashSha256) {
    if (rawHashSha256.size() !=  32) {
        QuasarAppUtils::Params::log("wrong parameters of the constructor NodeId!!",
                                    QuasarAppUtils::Error);
    }

    fromRaw(NODE_ID + rawHashSha256);
}

bool NodeId::isNodeId(const QByteArray &raw) {
    return raw.size() == 33 && raw[0] == NODE_ID;
}

bool NodeId::isValid() const {
    return BaseId::isValid() && toRaw()[0] == NODE_ID;
}
}
