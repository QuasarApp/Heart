//#
//# Copyright (C) 2025-2025 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "translatablestring.h"
#include "crc32constexper.h"
namespace QH {

TranslatableString::TranslatableString(const QString& data, bool isKey) {
    _data = data;
    _isKey = isKey;
}

bool TranslatableString::operator==(const TranslatableString &other) const {
    return _data == other._data && _isKey == other._isKey && _args == other._args;
}

bool TranslatableString::isValid() const {
    return _data.size();
}

bool TranslatableString::isEmpty() const {
    return _data.isEmpty();
}

QString TranslatableString::text() const {
    if (_isKey) {
        QString result = QObject::tr(_data.toLatin1());
        for (const auto& arg: _args) {
            result = result.arg(arg.text());
        }

        return result;
    }

    return _data;
}

const QString &TranslatableString::rawText() const {
    return _data;
}

int TranslatableString::calcCrc32Hash() const {
    return QuasarAppUtils::calculateCrc32(_data.toLatin1(), _data.size());
}

TranslatableString TranslatableString::tr(const QString &source) {
    return {source, true};
}

QDataStream &TranslatableString::fromStream(QDataStream &stream) {
    versionHeader(0, stream);

    readWrite(stream, _isKey);
    readWrite(stream, _data);
    readWrite(stream, _args);

    return stream;
}

QDataStream &TranslatableString::toStream(QDataStream &stream) const {
    versionHeader(0, stream);

    readWrite(stream, _isKey);
    readWrite(stream, _data);
    readWrite(stream, _args);

    return stream;
}

TranslatableString &TranslatableString::setArgs(const QList<TranslatableString> &newArgs) {
    _args = newArgs;

    return *this;
}
}
