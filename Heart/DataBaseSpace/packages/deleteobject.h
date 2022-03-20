#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H
/*
 * Copyright (C) 2018-2022 QuasarApp.
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
class HEARTSHARED_EXPORT DeleteObject: public DBObject, public IToken
{
    QH_PACKAGE(DeleteObject, "DeleteObject")

public:
    DeleteObject();
    DeleteObject(const Package& pkg);

    DBObject *createDBObject() const override;
    DBVariantMap variantMap() const override;
    const AccessToken &getSignToken() const override;
    void setSignToken(const AccessToken &token) override;

    bool fromSqlRecord(const QSqlRecord &q) override;

    bool isCached() const override;

    /**
     * @brief address This method return address of the object that will be deleted.
     * @return database address of the object that will be deleted.
     */
    const DbAddress &address() const;

    /**
     * @brief setAddress This method sets new address for object that will be deleted from database.
     * @param newAddress This is new value of the database adderess.
     */
    void setAddress(const DbAddress &newAddress);

protected:
    QString primaryKey() const override;
    QString primaryValue() const override;

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    DbAddress _address;
    AccessToken _token;

};
}
}
#endif // DELETEOBJECT_H
