#include "basenode.h"

namespace ClientProtocol {

BaseNode::BaseNode(ClientProtocol::SslMode mode, QObject *ptr):
    AbstractNode(mode, ptr) {

}

BaseNode::~BaseNode() {

}

}

