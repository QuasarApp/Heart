/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sslsocket.h"

#ifdef USE_HEART_SSL

namespace QH {

SslSocket::SslSocket(QObject *parent):
    QSslSocket(parent) {

    connect(this, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            this, [this](const QList<QSslError> & errors){
        emit sslErrorsOcurred(this, errors);
    }, Qt::DirectConnection);
}

SslSocket::~SslSocket() {
    handleDisckonnetFromHost();
}

void SslSocket::handleDisckonnetFromHost() {
    disconnectFromHost();
}

}
#endif
