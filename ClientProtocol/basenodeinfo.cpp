#include "basenodeinfo.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace ClientProtocol {

bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid() && !_token.isEmpty();
}

BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp, NodeType type):
    AbstractNodeInfo(tcp, type){}

BaseNodeInfo::~BaseNodeInfo(){}

QByteArray BaseNodeInfo::token() const {
    return _token;
}

void BaseNodeInfo::setToken(const QByteArray &token) {
    _token = token;
}

Permission BaseNodeInfo::permision() const {
    return _permision;
}

void BaseNodeInfo::setPermision(const Permission &permision) {
    _permision = permision;
}

}
