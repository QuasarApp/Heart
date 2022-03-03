/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef KNOWADDRESSES_H
#define KNOWADDRESSES_H

#include <QSet>
#include <abstractdata.h>
#include <nodeid.h>

namespace QH {
namespace PKG {

/**
 * @brief The KnowAddresses class - this class is package for send the list of know addresses of node to other node object.
 */
class HEARTSHARED_EXPORT KnowAddresses: public AbstractData
{
public:
    KnowAddresses();
    KnowAddresses(const Package &pkg);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    QSet<NodeId> knowAddresses() const;
    void setKnowAddresses(const QSet<NodeId> &knowAddresses);


    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QSet<NodeId> _knowAddresses;
};
}
}
#endif // KNOWADDRESSES_H
