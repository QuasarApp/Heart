/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "nodeobject.h"
#include <QCryptographicHash>

namespace QH {
namespace PKG {

NodeObject::NodeObject() {

}

NodeObject::NodeObject(const Package &pkg):
    NetworkMember(pkg) {

}

QDataStream &NodeObject::fromStream(QDataStream &stream) {

    NetworkMember::fromStream(stream);

    stream >> _senderID;

    return stream;
}

QDataStream &NodeObject::toStream(QDataStream &stream) const {
    NetworkMember::toStream(stream);

    stream << _senderID;

    return stream;
}

BaseId NodeObject::generateId() const {
    if (authenticationData().isEmpty()) {
        return {};
    }

    return QCryptographicHash::hash(authenticationData(), QCryptographicHash::Sha256);
}

DBVariantMap NodeObject::variantMap() const {
    return {{"authenticationData",  {authenticationData(),   MemberType::InsertOnly}},
            {"trust",               {trust(),                MemberType::InsertUpdate}}};
}

DBObject *NodeObject::createDBObject() const {
    return create<NodeObject>();
}

QPair<QString, QString> NodeObject::altarnativeKey() const {
    return {"authenticationData", authenticationData().toBase64(QByteArray::Base64UrlEncoding)};
}
}
}
