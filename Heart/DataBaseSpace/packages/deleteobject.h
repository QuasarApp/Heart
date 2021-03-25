#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H
/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include <dbobject.h>
#include <itoken.h>


namespace QH {
namespace PKG {

/**
 * @brief The DeleteObject class is request for update object with dbId;
 */
class DeleteObject: public DBObject, public IToken
{
public:
    DeleteObject();
    DeleteObject(const Package& pkg);

    DBObject *createDBObject() const override;
    DBVariantMap variantMap() const override;
    const AccessToken &getSignToken() const override;
    void setSignToken(const AccessToken &token) override;

protected:
    QString primaryKey() const override;

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    // DBObject interface
public:
    bool isCached() const override;

    AccessToken _token;

};
}
}
#endif // DELETEOBJECT_H
