#include "userdatarequest.h"

#include <QDataStream>

namespace NetworkProtocol {

UserDataRequest::UserDataRequest()
    : UserData() {
    generateId<decltype (this)>();

}

UserDataRequest::UserDataRequest(const Package &package):
    UserData(package) {
    generateId<decltype (this)>();

}

QDataStream &UserDataRequest::fromStream(QDataStream &stream) {
    UserData::fromStream(stream);
    stream >> requestCmd;

    return stream;
}

QDataStream &UserDataRequest::toStream(QDataStream &stream) const {
    UserData::toStream(stream);
    stream << requestCmd;

    return stream;
}

bool UserDataRequest::isValid() const {
    return  UserData::isValid() &&
            requestCmd > static_cast<unsigned char>(UserDataRequestCmd::Invalid) &&
            requestCmd <= static_cast<unsigned char>(UserDataRequestCmd::Delete);
}

void UserDataRequest::clear() {
    UserData::clear();
    requestCmd = static_cast<unsigned char>(UserDataRequestCmd::Invalid);
}
}
