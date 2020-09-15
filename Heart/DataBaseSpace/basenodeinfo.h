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
 * @brief The BaseNodeInfo class contaisn list of nodes id of know this node.
 */
class HEARTSHARED_EXPORT BaseNodeInfo: public AbstractNodeInfo {

public:

    /**
     * @brief BaseNodeInfo - create node info from the tcp descriptor
     * @param tcp - tcp socket dsscriptor
     */
    explicit BaseNodeInfo(QAbstractSocket * tcp = nullptr,
                          const HostAddress* clientAddress = nullptr);
    ~BaseNodeInfo() override;

    /**
     * @brief isValid
     * @return true if node is valid.
     */
    bool isValid() const override;

    /**
     * @brief selfId - it is id of peer node
     * @return
     */
    BaseId selfId() const;

    /**
     * @brief setSelfId
     * @param selfId
     */
    void setSelfId(const BaseId &selfId);

    /**
     * @brief confirmData - this implementaton check self id of node.
     * @return true if node contains valid self id.
     */
    bool confirmData() const override;

protected:
    BaseId _selfId;
};

}

#endif // CONNECTIONINFO_H
