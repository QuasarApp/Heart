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

namespace NP {

class DBObject;
class AbstractData;

class NETWORKPROTOCOLSHARED_EXPORT iObjectProvider
{
public:
    iObjectProvider();
    virtual ~iObjectProvider();

    /**
     * @brief getObject - this method return pointer to DBObject created by select method of template object.
     * @param obj - template object with a select db request.
     * @return return pointer to DBObject ot nullptr id object not exits.
     */
    template<class TYPE>
    TYPE *getObject(const TYPE &templateVal);

    /**
     * @brief getAllObjects - executable select method of objects and return list of all selected objects
     * @param obj - template object with select request.
     * @param result - return value, list of selected objects.
     * @return true if objects have in db else false.
     */
    virtual bool getAllObjects(const DBObject &templateObject,  QList<DBObject *> &result) = 0;

    /**
     * @brief saveObject
     * @return
     */
    virtual bool saveObject(const DBObject* saveObject) = 0;

    /**
     * @brief deleteObject
     * @return
     */
    virtual bool deleteObject(const DBObject* obj) = 0;

};

}
#endif // IOBJECTPROVIDER_H
