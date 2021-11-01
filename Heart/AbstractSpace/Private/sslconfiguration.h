/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SSLCONFIGURATION_H
#define SSLCONFIGURATION_H

#include "heart_global.h"

#ifdef USE_HEART_SSL

#include <QSslConfiguration>

namespace QH {

class SslConfiguration
{
public:
    SslConfiguration();

    const QSslConfiguration &sslConfig() const;
    void setSslConfig(const QSslConfiguration &newSslConfig);

    const QList<QSslError> &ignoreErrors() const;
    void setIgnoreErrors(const QList<QSslError> &newIgnoreErrors);

private:
    QSslConfiguration _sslConfig;
    QList<QSslError> _ignoreErrors;

};

}
#endif
#endif // SSLCONFIGURATION_H
