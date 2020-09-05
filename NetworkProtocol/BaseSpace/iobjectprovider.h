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
#include <dbobject.h>
#include <quasarapp.h>

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
    const TYPE *getObject(const TYPE &templateVal) {

        if (!dynamic_cast<const DBObject*>(&templateVal)) {
            return nullptr;
        }

        QList<const DBObject *> list;
        if (!getAllObjects(templateVal, list)) {
            return nullptr;
        }

        if (list.size() > 1) {
            QuasarAppUtils::Params::log("getObject method returned more than one object,"
                                        " the first object was selected as the result, all the rest were lost.",
                                        QuasarAppUtils::Warning);
        }

        for (int i = 1; i < list.size(); ++i ) {
            delete list[i];
        }

        const TYPE* result = dynamic_cast<const TYPE*>(list.first());
        if (!result && list.first()) {
            QuasarAppUtils::Params::log("getObject method returned object with deffirent type of TYPE,"
                                        " check getAllObjects merhod",
                                        QuasarAppUtils::Error);

            delete list.first();
        }

        return result;
    }

    /**
     * @brief getAllObjects - executable select method of objects and return list of all selected objects
     * @param obj - template object with select request.
     * @param result - return value, list of selected objects.
     * @return true if objects have in db else false.
     */
    virtual bool getAllObjects(const DBObject &templateObject,  QList<const DBObject *> &result) = 0;

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
