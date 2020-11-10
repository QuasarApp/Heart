/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include "abstractnodeinfo.h"
#include "accesstoken.h"
#include "baseid.h"
#include "heart_global.h"
#include <QByteArray>

class QAbstractSocket;
namespace QH {

class DbAddress;

/**
 * @brief The BaseNodeInfo class contaisn the unique node id.
 * This object created in the DataBaseNode::createNodeInfo method.
 */
class HEARTSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:

    /**
     * @brief BaseNodeInfo This constructor initialize the node info object from the tcp descriptor
     * @param tcp This is  tcp socket dsscriptor
     * @param clientAddress This is network address of socket.
     */
    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr,
                          const HostAddress* clientAddress = nullptr);
    ~BaseNodeInfo() override;

    bool isValid() const override;

    /**
     * @brief selfId This method return id of peer node or client.
     * @return The id of peer node or client.
     */
    const QVariant& selfId() const;

    /**
     * @brief setSelfId This method set an id for the peer node or the client.
     * @param selfId new value of id of the peer node.
     */
    void setSelfId(const BaseId &selfId);

    bool confirmData() const override;

protected:
    QVariant _selfId;
};

}

#endif // CONNECTIONINFO_H
