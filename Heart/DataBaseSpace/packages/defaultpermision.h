/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DEFAULTPERMISION_H
#define DEFAULTPERMISION_H

#include "memberpermisionobject.h"
#include "permission.h"


namespace QH {
namespace PKG {

/**
 * @brief The DefaultPermision class contains default of network members
 * This class works with the DefaultPermissions sql table.
 */
class HEARTSHARED_EXPORT DefaultPermision: public MemberPermisionObject
{
    QH_PACKAGE(DefaultPermision, "DefaultPermision")

public:
    DefaultPermision();
    DefaultPermision(const PermisionData& permision);

    DBVariantMap variantMap() const override;

    bool isValid() const override;

    bool fromSqlRecord(const QSqlRecord &q) override;
    DBObject *createDBObject() const override;
    QString condition() const override;
};
}
}
#endif // DEFAULTPERMISION_H
