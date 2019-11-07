#include "basenodeinfo.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace ClientProtocol {

bool BaseNodeInfo::isValid() const {
    return AbstractNodeInfo::isValid() && !_token.isValid();
}

BaseNodeInfo::BaseNodeInfo(QAbstractSocket *tcp, NodeType type):
    AbstractNodeInfo(tcp, type){}

BaseNodeInfo::~BaseNodeInfo(){}

const AccessToken &BaseNodeInfo::token() const {
    return _token;
}

void BaseNodeInfo::setToken(const AccessToken &token) {
    _token = token;
}

Permission BaseNodeInfo::permision(const QString &table, int id) const {
    auto allTablePermision = _permision.value(table).value(-1);
    auto itemPermision = _permision.value(table).value(id);

    return std::max(allTablePermision, itemPermision);
}

void BaseNodeInfo::setPermision(const QString &table, int id, const Permission &permision) {
    _permision[table][id] = permision;
}

}
