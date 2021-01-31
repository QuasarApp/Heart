#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H
/*
 * Copyright (C) 2018-2021 QuasarApp.
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

    DBObject *createDBObject() const override;
    DBVariantMap variantMap() const override;

protected:
    QString primaryKey() const override;

    // DBObject interface
public:
    bool isCached() const override;
};
}
}
#endif // DELETEOBJECT_H
