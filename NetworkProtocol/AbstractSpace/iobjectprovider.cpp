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

DBObject *iObjectProvider::getObject(DBObject *obj) {
    QList<DBObject *> list;
    if (!getAllObjects(obj, list)) {
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

    return list.first();
}

}
