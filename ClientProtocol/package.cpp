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

    auto rawint = data.mid(0, 4);
    unsigned int cmd;
    memcpy(&cmd, rawint.data(), sizeof (cmd));

    if (data.size() && hdr.command != cmd) {
        std::reverse(rawint.begin(), rawint.end());
        memcpy(&cmd, rawint.data(), sizeof (cmd));

        if (hdr.command != cmd)
            return false;
    }

    return hdr.size == data.size();
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
