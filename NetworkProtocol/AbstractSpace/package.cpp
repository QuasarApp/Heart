/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "abstractdata.h"
#include "package.h"

namespace NP {

Package::Package() {
    reset();
}

bool Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    auto rawint = data.mid(0, sizeof (decltype (hdr.command)));
    decltype (hdr.command) cmd;
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

void Package::fromBytes(const QByteArray& array) {
    reset();
    memcpy(&hdr,
           array.data(), sizeof(Header));

    data.append(array.mid(sizeof(Header)));
}

void Package::reset() {
    hdr.reset();
    data.clear();
}

}
