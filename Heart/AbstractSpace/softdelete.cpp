/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "softdelete.h"
#include <quasarapp.h>

QH::SoftDelete::SoftDelete() = default;

QH::SoftDelete::~SoftDelete() {

    if (!fSoftDelete) {

        QuasarAppUtils::Params::log("You delte ISqlDBCache without preparing. "
                                    "All changes can not be saved. "
                                    "For fix it trouble use the softDelete method.",
                                    QuasarAppUtils::Error);
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

