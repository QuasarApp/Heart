#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H
/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include <dbobject.h>


namespace QH {
namespace PKG {

/**
 * @brief The DeleteObject class is request for update object with dbId;
 */
class DeleteObject: public DBObject
{
public:
    DeleteObject();
    DeleteObject(const Package& pkg);

    // DBObject interface
public:
    DBObject *createDBObject() const override;

protected:
    DBVariantMap variantMap() const override;

    // DBObject interface
public:
    bool isCached() const override;
};
}
}
#endif // DELETEOBJECT_H
