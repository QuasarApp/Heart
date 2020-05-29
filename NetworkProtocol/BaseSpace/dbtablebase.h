/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBTABLEBASE_H
#define DBTABLEBASE_H

#include <QSet>
#include <QString>
#include <QVariantMap>

#include "networkprotocol_global.h"

namespace NP {

class DBObject;

/**
 * @brief The DbTableBase class
 */
struct NETWORKPROTOCOLSHARED_EXPORT DbTableBase
{
    QString name;
    QHash<QString, QVariant::Type> keys;

    /**
     * @brief lastIdQuery
     * @return
     */
    QString lastIdQuery() const;

    /**
     * @brief isInited
     * @return
     */
    bool isInited() const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

};
}



#endif // DBTABLEBASE_H
