#include "streammultiversion.h"
#include <QDataStream>

namespace QH {

StreamMultiversion::StreamMultiversion() {

}

void StreamMultiversion::saveVersion(char version, QDataStream &stream) const {
    stream << version;
}

char StreamMultiversion::readVersion(QDataStream &stream) const {
    char version;
    stream >> version;

    return version;

}
}
