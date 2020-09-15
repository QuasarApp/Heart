/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKCONNECTIONINFO_H
#define NETWORKCONNECTIONINFO_H

#include "basenodeinfo.h"
#include "accesstoken.h"
#include "baseid.h"
#include "heart_global.h"
#include <QByteArray>

class QAbstractSocket;
namespace NP {

class DbAddress;

/**
 * @brief The BaseNodeInfo class contaisn list of nodes id of know this node.
 */
class NETWORKPROTOCOLSHARED_EXPORT NetworkNodeInfo: public BaseNodeInfo {

public:

    /**
     * @brief NetworkNodeInfo - create node info from the tcp descriptor
     * @param tcp - tcp socket dsscriptor
     */
    explicit NetworkNodeInfo(QAbstractSocket * tcp = nullptr,
                          const HostAddress* clientAddress = nullptr);
    ~NetworkNodeInfo() override;

    /**
     * @brief isValid
     * @return true if node is valid.
     */
    bool isValid() const override;

    /**
     * @brief isKnowAddress
     * @param address
     * @return
     */
    bool isKnowAddress(const BaseId& address) const;

    /**
     * @brief setSelfId
     * @param selfId
     */
    void setSelfId(const BaseId &selfId);

    /**
     * @brief addKnowAddresses
     */
    void addKnowAddresses(const QSet<BaseId> &newAddressses);

    /**
     * @brief confirmData - this implementaton check self id of node.
     * @return true if node contains valid self id.
     */
    bool confirmData() const override;

protected:
    QSet<BaseId> _knowAddresses;
};

}

#endif // NETWORKCONNECTIONINFO_H
