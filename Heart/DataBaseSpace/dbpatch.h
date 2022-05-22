/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBPATCH_H
#define DBPATCH_H

#include "iobjectprovider.h"


namespace QH {


/**
 * @brief DBPatch This is function that should be upgrade database.
 * @see DBPatchMap
 * @see DataBaseNode::dbPatch
 *
 * @note version is version number of the current database.
 * After execute this path version of data base will be increment.
 */
struct DBPatch {
    /// This is version of data base that need to up.
    /// For example - we has a data base with version 1 and we want to upgrade to version 2 then you need to set cersion field version to 1.
    unsigned int version = 0;

    /// This is lymbda function with action that will upgrade data base to new versio.
    std::function<bool (const QH::iObjectProvider *)> action;
};

/**
 * @brief DBPatchMap This is map when key of list is version of database and value if function that should be upgrade database.
 * @see DataBaseNode::dbPatch
 * @see DBPatchMap
 */
typedef QMap<unsigned int, DBPatch> DBPatchMap;
}
#endif // DBPATCH_H
