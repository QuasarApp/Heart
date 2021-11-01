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

/**
 * @brief The SslConfiguration class This is simple class for contains ssl configurations.
 */
class SslConfiguration
{
public:
    SslConfiguration();

    /**
     * @brief sslConfig This method return current qt ssl configuration.
     * @return  current qt ssl configuration.
     */
    const QSslConfiguration &sslConfig() const;

    /**
     * @brief setSslConfig This method sets new ssl configuration.
     * @param newSslConfig This is a new ssl configuration.
     */
    void setSslConfig(const QSslConfiguration &newSslConfig);

    /**
     * @brief ignoreErrors This method return list of ignored ssl errors.
     * @return list of ignored ssl errors.
     */
    const QList<QSslError> &ignoreErrors() const;

    /**
     * @brief setIgnoreErrors This nethod sets new list of ignored ssl errors.
     * @param newIgnoreErrors this is a new value for ignored ssl errors.
     */
    void setIgnoreErrors(const QList<QSslError> &newIgnoreErrors);

private:
    QSslConfiguration _sslConfig;
    QList<QSslError> _ignoreErrors;

};

}
#endif
#endif // SSLCONFIGURATION_H
