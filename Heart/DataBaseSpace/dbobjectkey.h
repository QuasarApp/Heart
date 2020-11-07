/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECTKEY_H
#define DBOBJECTKEY_H

#include "abstractkey.h"
namespace QH {

namespace PKG {
class DBObject;
}

/**
 * @brief The DBObjectKey class This class calc hash from DBOject.
 * Ditals:
 * This class using the header of sql query for generate hash code of object.
 */
class DBObjectKey: public AbstractKey
{
public:
    DBObjectKey(const PKG::DBObject* dbObjcet);

    unsigned int hash() const override;
    const BaseId *id() const override;
    const QString *table() const override;
    bool isValid() const override;

private:
    const PKG::DBObject* _source;
};
}
#endif // DBOBJECTKEY_H
