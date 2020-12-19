/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "iobjectprovider.h"

namespace QH {
using namespace PKG;

QSharedPointer<DBObject> iObjectProvider::getObjectRaw(
        const DBObject &templateVal) {

    if (!dynamic_cast<const DBObject*>(&templateVal)) {
        return nullptr;
    }

    QList<QSharedPointer<PKG::DBObject>> list;
    if (!getAllObjects(templateVal, list)) {
        return nullptr;
    }

    if (list.size() > 1) {
        QuasarAppUtils::Params::log("getObject method returned more than one object,"
                                    " the first object was selected as the result, all the rest were lost.",
                                    QuasarAppUtils::Warning);
    }

    return list.first();
}

}
