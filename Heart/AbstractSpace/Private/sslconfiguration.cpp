/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "sslconfiguration.h"

#ifdef USE_HEART_SSL

namespace QH {

SslConfiguration::SslConfiguration()
{

}

const QSslConfiguration &SslConfiguration::sslConfig() const {
    return _sslConfig;
}

void SslConfiguration::setSslConfig(const QSslConfiguration &newSslConfig) {
    _sslConfig = newSslConfig;
}

const QList<QSslError> &SslConfiguration::ignoreErrors() const {
    return _ignoreErrors;
}

void SslConfiguration::setIgnoreErrors(const QList<QSslError> &newIgnoreErrors) {
    _ignoreErrors = newIgnoreErrors;
}

}

#endif
