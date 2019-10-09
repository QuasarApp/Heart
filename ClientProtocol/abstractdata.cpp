#include "abstractdata.h"
#include <QDataStream>
#include <typeinfo>
#include "clientprotocol.h"

namespace ClientProtocol {



static QMap<size_t, unsigned char> commandTable = {};


unsigned char ClientProtocol::AbstractData::cmd() const {
    return _cmd;
}

AbstractData::AbstractData() {
    size_t hash = typeid(*this).hash_code();
    if (!commandTable.contains(hash)) {
        commandTable[hash] = static_cast<unsigned char>(commandTable.size());
    }
    _cmd = commandTable[hash];
}

AbstractData::AbstractData(const ClientProtocol::BasePackage &package):
    AbstractData() {

    fromBytes(package.data);
}

bool AbstractData::fromBytes(const QByteArray &data) {

    if (data.isEmpty())
        return false;

    QDataStream stream(data);
    fromStream(stream);
    return true;
}

QByteArray AbstractData::toBytes() const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    toStream(stream);
    return res;
}

bool AbstractData::toPackage(BasePackage &package,
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

QDataStream &AbstractData::fromStream(QDataStream &stream) {
    stream >> _cmd;
    return stream;
}

QDataStream &AbstractData::toStream(QDataStream &stream) const {
    stream << _cmd;
    return stream;
}

bool AbstractData::isValid() const {
    return _cmd;
}

}
