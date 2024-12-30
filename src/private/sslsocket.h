/*
 * Copyright (C) 2018-2025 QuasarApp.
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

signals:

    /**
     * @brief sslErrorsOcurred This is wrapper signal of the QSslSocket::sslErrors method.
     * @param sender This is pointer to sender object.
     * @param errors Thiis is list of ssl errors.
     */
    void sslErrorsOcurred(SslSocket* sender, const QList<QSslError> &errors);
};
}
#endif
#endif // SSLSOCKET_H


