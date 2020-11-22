/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "iobjectprovider.h"

namespace QH {
using namespace PKG;
iObjectProvider::iObjectProvider() = default;

iObjectProvider::~iObjectProvider() = default;

Promise<DBObject> iObjectProvider::getObject(const DBObject &templateVal) {

    Promise<DBObject> result;
    if (!dynamic_cast<const DBObject*>(&templateVal)) {
        result.reject();
        return result;
    }

    Promise<QList<const DBObject *>> promise;
    if (!getAllObjects(templateVal, promise)) {
        result.reject();
        return result;
    }

    bool subscribe = promise.subscribe([result](const QList<const DBObject *>& objects) mutable {

        if (!objects.size()) {
            result.reject();
            return;
        }

        if (objects.size() > 1) {
            QuasarAppUtils::Params::log("getObject method returned more than one object,"
                                        " the first object was selected as the result, all the rest were lost.",
                                        QuasarAppUtils::Warning);
        }

        result.setValue(objects.first());


    });

    if (!subscribe) {
        result.reject();
        return result;
    }


    return result;
}

}
