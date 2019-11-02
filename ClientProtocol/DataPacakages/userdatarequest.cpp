#include "userdatarequest.h"

#include <QDataStream>

namespace ClientProtocol {

UserDataRequest::UserDataRequest()
    : UserData() {

}

UserDataRequest::UserDataRequest(const Package &package):
    UserData(package) {

}

UserDataRequestCmd UserDataRequest::requestCmd() const {
    return _requestCmd;
}

void UserDataRequest::setRequestCmd(const UserDataRequestCmd &requestCmd) {
    _requestCmd = requestCmd;
}

QDataStream &UserDataRequest::fromStream(QDataStream &stream) {
    UserData::fromStream(stream);
    unsigned char cmd;
    stream >> cmd;
    _requestCmd = static_cast<UserDataRequestCmd>(cmd);

    return stream;
}

QDataStream &UserDataRequest::toStream(QDataStream &stream) const {
    UserData::toStream(stream);
    stream << static_cast<unsigned char>(_requestCmd);

    return stream;
}

bool UserDataRequest::isValid() const {
    return  UserData::isValid() &&
            _requestCmd > UserDataRequestCmd::Invalid &&
            _requestCmd <= UserDataRequestCmd::Delete;
}

void UserDataRequest::clear() {
    UserData::clear();
    _requestCmd = UserDataRequestCmd::Invalid;
}
}
