#include "clientprotocol.h"

#include <QDataStream>
#include <QVariantMap>

namespace ClientProtocol {

BaseHeader::BaseHeader() {
    reset();
}

bool BaseHeader::isValid() const {

    if (sizeof (*this) != 4) {
        return false;
    }

    return true;
}

void BaseHeader::reset() {
    size = 0;
    command = 0;
    triggerCommnad = 0;
}

BasePackage::BasePackage() {
    reset();
}

bool BasePackage::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    if (data.size() && hdr.command != data.at(0)) {
        return false;
    }

    return hdr.size == static_cast<unsigned int> (data.size());
}

QByteArray BasePackage::toBytes() const {
    QByteArray res;
    res.append(reinterpret_cast<char*>(const_cast<BaseHeader*>(&hdr)),
               sizeof (hdr));

    res.append(data);
    return res;
}

void BasePackage::reset() {
    hdr.reset();
    data.clear();
}

bool initClientProtockol() {
    return true;
}

}
