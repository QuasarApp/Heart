/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "iobjectprovider.h"
#include <dbobject.h>
#include <quasarapp.h>

namespace NP {
iObjectProvider::iObjectProvider() = default;

iObjectProvider::~iObjectProvider() = default;

template<class TYPE>
TYPE *iObjectProvider::getObject(const TYPE &templateVal) {

    if (!dynamic_cast<DBObject*>(&templateVal)) {
        return nullptr;
    }

    QList<DBObject *> list;
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

    TYPE* result = dynamic_cast<TYPE*>(list.first());
    if (!result && list.first()) {
        QuasarAppUtils::Params::log("getObject method returned object with deffirent type of TYPE,"
                                    " check getAllObjects merhod",
                                    QuasarAppUtils::Error);

        delete list.first();
    }

    return result;
}

}
