/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "badrequest.h"

#include <QDataStream>

namespace QH{
namespace PKG {


BadRequest::BadRequest(unsigned char errocode, const QString &err) {
    setErr(err);
    setErrCode(errocode);
}

BadRequest::BadRequest(const ErrorData &data):
    BadRequest(data.code, data.msg) {
}

BadRequest::BadRequest(const Package &package):
    BadRequest() {
    fromBytes(package.data);
}

QDataStream &BadRequest::fromStream(QDataStream &stream) {
    stream >> _errCode;
    stream >> _err;

    return stream;
}

QDataStream &BadRequest::toStream(QDataStream &stream) const {

    stream << _errCode;
    stream << _err;

    return stream;
}

unsigned char BadRequest::errCode() const {
    return _errCode;

}

void BadRequest::setErrCode(unsigned char code) {
    _errCode = code;
}

QString BadRequest::err() const {
    return _err;
}

void BadRequest::setErr(const QString &err) {
    _err = err;
}
}
}
