#include "usernodeinfo.h"
namespace NP {

UserNodeInfo::UserNodeInfo(QAbstractSocket *tcp): BaseNodeInfo(tcp) {

}

bool UserNodeInfo::isValid() const {
    return BaseNodeInfo::isValid();
}

unsigned int UserNodeInfo::destUserId() const {
    return _destUserId;
}

void UserNodeInfo::setDestUserId(unsigned int destUserId) {
    _destUserId = destUserId;
}

UserBaseData *UserNodeInfo::getCurrentUserData() const {
    return currentUserData;
}

void UserNodeInfo::setCurrentUserData(UserBaseData *value) {
    currentUserData = value;
}

}
