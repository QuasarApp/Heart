/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef TRANSPORTDATA_H
#define TRANSPORTDATA_H

#include "abstractdata.h"
#include "nodeid.h"
#include "senderdata.h"

#include "hostaddress.h"

namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT TransportData: public AbstractData, public SenderData
{

public:
    explicit TransportData();
    explicit TransportData(const Package& package);

    const Package& data() const;
    void setData(const Package &data);
    void setData(const AbstractData& data);
    bool isValid() const;

    /**
     * @brief targetAddress - targetAddress
     * @return
     */
    BaseId targetAddress() const;
    void setTargetAddress(const BaseId &targetAddress);

    /**
     * @brief route - is list of addresses of node was the TransportData has been moved.
     * @return
     */
    const QList<HostAddress> &route() const;
    void setRoute(const QList<HostAddress> &route);
    void addNodeToRoute(const HostAddress &route);

    /**
     * @brief strip - this method remove a range addresses from route of transport data. from correntAddress to availabelTarget
     * @param correntAddress - begin of remove
     * @param availabelTarget - end of remove
     * @return true if all addresses is removed
     */
    bool strip(const HostAddress& correntAddress, const HostAddress& availabelTarget);

protected:
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    Package _data;
    BaseId _targetAddress;
    QList<HostAddress> _route;


};

}
#endif // TRANSPORTDATA_H
