#include "authrequest.h"

#include <QDataStream>

namespace QH {


namespace PKG {
AuthRequest::AuthRequest() {

}

AuthRequest::AuthRequest(const Package &pkg):UserMember(pkg) {

}

unsigned char AuthRequest::getRequestCmd() const {
    return static_cast<unsigned char>(_request);
}

QDataStream &AuthRequest::fromStream(QDataStream &stream) {
    UserMember::fromStream(stream);
    stream >> _request;
    return stream;
}

QDataStream &AuthRequest::toStream(QDataStream &stream) const {
    UserMember::toStream(stream);
    stream << _request;
    return stream;
}


void AuthRequest::setRequest(const UserRequestType &request) {
    _request = request;
}

}
}
