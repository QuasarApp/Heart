/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "defaultpermision.h"
namespace QH {
namespace PKG {

DefaultPermision::DefaultPermision():
    MemberPermisionObject("DefaultPermissions") {

}

DefaultPermision::DefaultPermision(const PermisionData &permision):
    DefaultPermision() {
    setKey(permision);
}

DBVariantMap DefaultPermision::variantMap() const {
    return {{"dbAddress",              {key().addressHash(),         MemberType::PrimaryKey}},
            {"lvl",                    {static_cast<unsigned char>(permisions()), MemberType::InsertUpdate}}};
}

bool DefaultPermision::isValid() const {
    return key().addressHash().size();
}

DBObject *DefaultPermision::createDBObject() const {
    return create<DefaultPermision>();
}

bool DefaultPermision::fromSqlRecord(const QSqlRecord &q) {
    PermisionData permision({}, q.value("dbAddress").toString());

    setKey(permision);

    setPermisions(static_cast<Permission>(q.value("lvl").toUInt()));
    return isValid();
}

}
}
