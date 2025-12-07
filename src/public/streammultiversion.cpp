#include "streammultiversion.h"
#include <QDataStream>

namespace QH {

StreamMultiversion::StreamMultiversion() {

}

StreamMultiversion::~StreamMultiversion() {

}

void StreamMultiversion::saveVersion(char version, QDataStream &stream) const {
    stream << version;
}

char StreamMultiversion::readVersion(QDataStream &stream) {
    char version;
    stream >> version;

    return version;

}

char StreamMultiversion::versionHeader(char, QDataStream &stream) {
    return readVersion(stream);
}

char StreamMultiversion::versionHeader(char version, QDataStream &stream) const {
    saveVersion(version, stream);
    return version;

}


}
