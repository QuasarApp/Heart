#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H
/*
 * Copyright (C) 2018-2023 QuasarApp.
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
 *
 * Example of use
 *
 * @code{cpp}
 * QH::PKG::DeleteObject rquest({"UsersTable", "userName"}, "name");
 * @endcode
 */
class HEARTSHARED_EXPORT DeleteObject: public DBObject, public IToken
{

public:
    DeleteObject();

    /**
     * @brief DeleteObject This is main constructor for request, you can remove any db object if you know them value of the primary key.
     * @param address This is defautl addres of the ovject {table, id}. Where id is value of the primart key.
     * @param primaryKey This is primary key name - filed of the table. By default using **id**
     */
    DeleteObject(const DbAddress& address, const QString& primaryKey);

    DeleteObject(const Package& pkg);

    DBObject *createDBObject() const override;
    DBVariantMap variantMap() const override;
    const AccessToken &getSignToken() const override;
    void setSignToken(const AccessToken &token) override;

    bool fromSqlRecord(const QSqlRecord &q) override;

    bool isCached() const override;

    QString table() const override;

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

    /**
     * @brief setPrimaryKey This meethod sets new value of the primary key.
     * @param newPrimaryKey This is new value of the primary key
     */
    void setPrimaryKey(const QString &newPrimaryKey);

protected:
    QString primaryKey() const override;
    QVariant primaryValue() const override;

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QString _primaryKey = "id";
    DbAddress _address;
    AccessToken _token;

};
}
}
#endif // DELETEOBJECT_H
