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

    explicit TransportData(const HostAddress& sender);
    explicit TransportData(const Package& package);

    const Package& data() const;
    void setData(const Package &data);
    void setData(const AbstractData& data);
    bool isValid() const;
    bool isHaveRoute() const;

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

    /**
     * @brief setRoute - set and validate route.
     * @param route
     * @return true if a new route is valid
     */
    bool setRoute(const QList<HostAddress> &route);
    void addNodeToRoute(const HostAddress &route);

    /**
     * @brief strip - this method remove a range addresses from route of transport data. from correntAddress to availabelTarget
     * @param correntAddress - begin of remove
     * @param availabelTarget - end of remove
     * @return true if all addresses is removed
     */
    bool strip(const HostAddress& correntAddress, const HostAddress& availabelTarget);

    /**
     * @brief packageId
     * @return unique package id.
     */
    BaseId packageId() const;

    /**
     * @brief completeRoute - set bool variable of route complete.
     * @param fRouteIsComplete - new value for complete route default is true
     */
    void completeRoute(bool fRouteIsComplete = true);

protected:
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    explicit TransportData();

    Package _data;
    BaseId _targetAddress;
    QList<HostAddress> _route;
    BaseId _packageId;
    bool _fRouteIsComplete = false;



};

}
#endif // TRANSPORTDATA_H
