#include "abstractdata.h"
#include "package.h"

namespace ClientProtocol {

Package::Package() {
    reset();
}

bool Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    if (data.size() && hdr.command != data.at(0)) {
        return false;
    }

    return hdr.size == static_cast<unsigned int> (data.size());
}

QByteArray Package::toBytes() const {
    QByteArray res;
    res.append(reinterpret_cast<char*>(const_cast<Header*>(&hdr)),
               sizeof (hdr));

    res.append(data);
    return res;
}

void Package::reset() {
    hdr.reset();
    data.clear();
}

}
