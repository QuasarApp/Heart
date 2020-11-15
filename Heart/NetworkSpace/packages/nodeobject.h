/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NODEOBJECT_H
#define NODEOBJECT_H

#include <networkmember.h>
#include <senderdata.h>
namespace QH {
namespace PKG {

/**
 * @brief The NodeObject class
 */
class HEARTSHARED_EXPORT NodeObject: public NetworkMember, public SenderData
{
public:
    NodeObject();
    NodeObject(const Package& pkg);

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    bool init() override;

public:
    DBObject *createDBObject() const override;
    bool copyFrom(const AbstractData *other) override;
};
}
}
#endif // NODEOBJECT_H
