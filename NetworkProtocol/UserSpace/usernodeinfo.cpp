#include "usernodeinfo.h"
namespace NP {

UserNodeInfo::UserNodeInfo(QAbstractSocket *tcp): BaseNodeInfo(tcp) {

}

bool UserNodeInfo::isValid() const {
    return BaseNodeInfo::isValid();
}

unsigned int UserNodeInfo::userId() const {
    return _userId;
}

void UserNodeInfo::setUserId(unsigned int userId) {
    _userId = userId;
}
}
