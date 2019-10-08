#include "map.h"
#include <clientprotocol.h>

namespace ClientProtocol {

Map::Map()
{
    _class = static_cast<quint8>(Command::Map);
}

BaseNetworkObject *Map::create() const {
    return new Map();
}

NetworkClassSize Map::classSize() const {

    return BaseNetworkObject::classSize()
            + getTypeSize(lvl)
            + getTypeSize(lenght)
            + getTypeSize(objects);
}

QDataStream &Map::writeToStream(QDataStream &stream) const {
    BaseNetworkObject::writeToStream(stream);
    stream << lvl;
    stream << lenght;
    stream << objects;

    return stream;
}

QDataStream &Map::readFromStream(QDataStream &stream) {
    BaseNetworkObject::readFromStream(stream);
    stream >> lvl;
    stream >> lenght;
    stream >> objects;
    return stream;
}

bool Map::isValid() const {
    return lenght > 1 && objects.size() > 1 &&
            BaseNetworkObject::isValid();
}

}
