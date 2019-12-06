#include "availabledatarequest.h"
namespace NP {

AvailableDataRequest::AvailableDataRequest() {
    INIT_COMMAND
}


AvailableDataRequest::AvailableDataRequest(const Package &pkg):AvailableData(pkg) {
    INIT_COMMAND
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
