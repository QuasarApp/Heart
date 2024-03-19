#include "streammultiversion.h"
#include <QDataStream>

namespace QH {

StreamMultiversion::StreamMultiversion() {

}

QDataStream &StreamMultiversion::fromStream(QDataStream &stream) {
    stream >> _realVersion ;
    return stream;
}

QDataStream &StreamMultiversion::toStream(QDataStream &stream) const {
    stream << _realVersion;

    return stream;
}

int StreamMultiversion::realVersion() const {
    return _realVersion;
}
}
