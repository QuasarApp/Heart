#include "abstractnodeinfo.h"
#include <QHostAddress>
#include <QAbstractSocket>

namespace ClientProtocol {

AbstractNodeInfo::AbstractNodeInfo(QAbstractSocket *sct, ClientProtocol::NodeType type) {
    setSct(sct);
    setType(type);
}

AbstractNodeInfo::~AbstractNodeInfo() {
    if (_sct) {
        _sct->deleteLater();
    }
}

QAbstractSocket *AbstractNodeInfo::sct() const {
    return _sct;
}

void AbstractNodeInfo::disconnect() {
    if (_sct) {
        _sct->close();
        _sct->deleteLater();
        _sct = nullptr;
        _type = NodeType::Undefined;
    }
}

unsigned int AbstractNodeInfo::id() const {
    if (_sct)
        return (qHash(_sct->peerAddress()));

    return 0;
}

void AbstractNodeInfo::ban() {
    _trust = static_cast<int>(TrustNode::Baned);
    disconnect();
}

bool AbstractNodeInfo::isBaned() const {
    return _trust < 1;
}

void AbstractNodeInfo::unBan() {
    _trust = static_cast<int>(TrustNode::Restore);
}

void AbstractNodeInfo::setSct(QAbstractSocket *sct) {
    _sct = sct;
}

int AbstractNodeInfo::trust() const {
    return _trust;
}

void AbstractNodeInfo::setTrust(int trust) {
    _trust = trust;

    if (isBaned()) {
        disconnect();
    }
}

NodeType AbstractNodeInfo::type() const {
    return _type;
}

void AbstractNodeInfo::setType(const NodeType &type) {
    _type = type;
}

bool AbstractNodeInfo::isValid() const {
    return _sct && _type != NodeType::Undefined;
}

}
