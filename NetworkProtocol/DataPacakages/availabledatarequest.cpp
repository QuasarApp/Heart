#include "availabledatarequest.h"
namespace NetworkProtocol {

AvailableDataRequest::AvailableDataRequest() {
    generateId<decltype (this)>();
}


AvailableDataRequest::AvailableDataRequest(const Package &pkg):AvailableData(pkg) {
    generateId<decltype (this)>();
}
AccessToken AvailableDataRequest::token() const
{
    return _token;
}

void AvailableDataRequest::setToken(const AccessToken &token)
{
    _token = token;
}

QDataStream &AvailableDataRequest::fromStream(QDataStream &stream) {
    AvailableData::fromStream(stream);

    stream >> _token;

    return stream;
}

QDataStream &AvailableDataRequest::toStream(QDataStream &stream) const {
    AvailableData::toStream(stream);

    stream << _token;

    return stream;
}

}
