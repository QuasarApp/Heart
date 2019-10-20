#include "abstract.h"
#include <QDataStream>
#include <QMap>
#include <typeinfo>
#include "clientprotocol.h"

namespace ClientProtocol {

namespace Data {


static QMap<size_t, unsigned char> commandTable = {};


unsigned char Abstract::cmd() const {
    return _cmd;
}

Abstract::Abstract() {
    size_t hash = typeid(*this).hash_code();
    if (!commandTable.contains(hash)) {
        commandTable[hash] = static_cast<unsigned char>(commandTable.size());
    }
    _cmd = commandTable[hash];
}

Abstract::Abstract(const ClientProtocol::BasePackage &package):
    Abstract() {

    fromBytes(package.data);
}

bool Abstract::fromBytes(const QByteArray &data) {

    if (data.isEmpty())
        return false;

    QDataStream stream(data);
    fromStream(stream);
    return true;
}

QByteArray Abstract::toBytes() const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    toStream(stream);
    return res;
}

bool Abstract::toPackage(BasePackage &package,
                                             unsigned char trigeredCommand) const {

    if (!isValid()) {
        return false;
    }

    package.data = toBytes();

    package.hdr.command = _cmd;
    package.hdr.triggerCommnad = trigeredCommand;
    package.hdr.size = static_cast<unsigned short>(package.data.size());

    return package.isValid();
}

QDataStream &Abstract::fromStream(QDataStream &stream) {
    stream >> _cmd;
    return stream;
}

QDataStream &Abstract::toStream(QDataStream &stream) const {
    stream << _cmd;
    return stream;
}

bool Abstract::isValid() const {
    return _cmd;
}

}

}
