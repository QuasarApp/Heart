#include "abstractdata.h"
#include <QDataStream>
#include <QMap>
#include <typeinfo>
#include "clientprotocol.h"

namespace ClientProtocol {


static QMap<size_t, unsigned char> commandTable = {};


unsigned char AbstractData::cmd() const {
    return _cmd;
}

AbstractData::AbstractData() {
    _cmd = static_cast<unsigned char>(generateId());
}

AbstractData::AbstractData(const ClientProtocol::Package &package):
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

int AbstractData::generateId() {
    return typeid(*this).hash_code() % 0xFF;
}

QByteArray AbstractData::toBytes() const {
    QByteArray res;
    QDataStream stream(&res, QIODevice::WriteOnly);
    toStream(stream);
    return res;
}

bool AbstractData::toPackage(Package &package,
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

AbstractData::~AbstractData() {

}


}
