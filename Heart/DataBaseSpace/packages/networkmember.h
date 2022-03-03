/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKMEMBER_H
#define NETWORKMEMBER_H

#include <abstractnetworkmember.h>

namespace QH {
namespace PKG {

/**
 * @brief The NetworkMember class is base implementation of the AbstractNetworkMember.
 * This class contains all default settings of the AbstractNetworkMember class.
 * If you want to create a own member class then use the AbstractNetworkMember class.
 */
class HEARTSHARED_EXPORT NetworkMember: public AbstractNetworkMember
{
    QH_PACKAGE(NetworkMember, "NetworkMember")

public:
    NetworkMember();
    NetworkMember(const QVariant& id);

public:
    DBObject *createDBObject() const override;
};

}
}
#endif // NETWORKMEMBER_H
