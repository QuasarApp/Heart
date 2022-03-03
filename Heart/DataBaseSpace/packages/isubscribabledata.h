/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ISUBSCRIBABLEDATA_H
#define ISUBSCRIBABLEDATA_H
#include "heart_global.h"
#include <QMetaType>

namespace QH {

namespace PKG {


/**
 * @brief The ISubscribableData class is interface provide subscribe functionality.
 * If you want to create a data object with support of the subscribe functionality then you need to inherit form this class.
 */
class HEARTSHARED_EXPORT ISubscribableData
{
public:
    ISubscribableData();

    /**
     * @brief subscribeId All subscribable object should be override this method.
     *  This method should be return signed int id.
     * @return unique id of the subscribe object.
     */
    virtual unsigned int subscribeId() const = 0;
};
}
}

#endif // ISUBSCRIBABLEDATA_H
