/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "networkprotocol_global.h"

#include <QSharedPointer>
#include "defines.h"

namespace NP {

class DBObject;
class AbstractData;

class NETWORKPROTOCOLSHARED_EXPORT iObjectProvider
{
public:
    iObjectProvider();
    virtual ~iObjectProvider();

    /**
     * @brief getObject
     * obj - dbobject with own query of select.
     * After the invoke this object well contain current data.
     * @return
     */
    virtual bool getObject(SP<DBObject>& obj) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(const WP<AbstractData>& saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(const WP<AbstractData>& saveObject) = 0;

};

}
#endif // IOBJECTPROVIDER_H
