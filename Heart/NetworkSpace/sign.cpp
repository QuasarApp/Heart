/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "sign.h"
namespace QH {

Sign::Sign()
{

}

QByteArray Sign::sign() const {
    return _sign;
}

void Sign::setSign(const QByteArray &sign) {
    _sign = sign;
}
}
