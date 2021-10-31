/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include "heart_global.h"

#ifdef USE_HEART_SSL

#include <QSslSocket>

namespace QH {

/**
 * @brief The SslSocket class This is wrapper of the ssl socket.
 */
class SslSocket: public QSslSocket
{
    Q_OBJECT
public:
    SslSocket(QObject *parent);
    ~SslSocket();
public slots:

    /**
     * @brief handleDisckonnetFromHost This is slot wrapper of the disconectFromHost method of QAbstractSocket class.
     */
    void handleDisckonnetFromHost();
};

#endif
#endif // SSLSOCKET_H

}
