#include "badrequest.h"

#include <QDataStream>

namespace NetworkProtocol{


BadRequest::BadRequest(const QString &err):AbstractData() {
    setErr(err);
}

BadRequest::BadRequest(const Package &package):
    AbstractData(package) {

}

QDataStream &BadRequest::fromStream(QDataStream &stream) {
    AbstractData::fromStream(stream);

    stream >> _err;

    return stream;
}

QDataStream &BadRequest::toStream(QDataStream &stream) const {
    AbstractData::toStream(stream);

    stream << _err;

    return stream;
}

QString BadRequest::err() const {
    return _err;
}

void BadRequest::setErr(const QString &err) {
    _err = err;
}

}
