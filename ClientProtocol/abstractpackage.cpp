#include "abstract.h"
#include "abstractpackage.h"
namespace ClientProtocol {

AbstractPackage::AbstractPackage() {
    reset();
}

bool AbstractPackage::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    if (data.size() && hdr.command != data.at(0)) {
        return false;
    }

    return hdr.size == static_cast<unsigned int> (data.size());
}

QByteArray AbstractPackage::toBytes() const {
    QByteArray res;
    res.append(reinterpret_cast<char*>(const_cast<AbstractHeader*>(&hdr)),
               sizeof (hdr));

    res.append(data);
    return res;
}

void AbstractPackage::reset() {
    hdr.reset();
    data.clear();
}

}
