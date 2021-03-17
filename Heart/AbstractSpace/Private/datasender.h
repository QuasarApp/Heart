/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DATASENDER_H
#define DATASENDER_H

#include "async.h"


class QAbstractSocket;

namespace QH {

/**
 * @brief The DataSender class this class create a queue for sendet data to network.
 */
class DataSender: public Async
{
    Q_OBJECT
public:
    DataSender();

    /**
     * @brief sendPackagePrivate This slot move send package to a main thread.
     * @param array Bytes to send.
     * @param target This is pointer of target socket.
     * @param await This option force wait for finishing data sending.
     */
    bool sendData(const QByteArray &array, void *target, bool await = false) const;

private:

    /**
     * @brief sendPackagePrivate This slot move send package to a main thread.
     * @param array Bytes to send
     * @param target - This is pointer of target socket
     */
    bool sendPackagePrivate(QByteArray array, void *target) const;
};
}
#endif // DATASENDER_H
