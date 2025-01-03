/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "softdelete.h"
#include <quasarapp.h>

QH::SoftDelete::SoftDelete() = default;

QH::SoftDelete::~SoftDelete() {

    if (!fSoftDelete) {

        qCritical() << "You delete %0 without preparing. " << typeid (this).name() <<
            "The object was deleted without preparing. All changes can not be saved. "
                       "For fix it trouble use the softDelete method.";

#ifdef QT_DEBUG
        std::abort();
#endif

    }
}

void QH::SoftDelete::softDelete() {
    fSoftDelete = true;
    prepareForDelete();
    delete this;
}

