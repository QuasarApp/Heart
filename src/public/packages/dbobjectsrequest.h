/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECTSREQUEST_H
#define DBOBJECTSREQUEST_H

#include "dbobjectset.h"

#include <QDataStream>


namespace QH {
namespace PKG {

/**
 * @brief The DBObjectsRequest class is template class for get array of TEMPLATE Objects from database.
 *
 * Example for using this class.
 *
 * \code{cpp}
 *  DBObjectsRequest<User> query("users", "id='Andrei'");
 *  SingleServer::getObject(query);
 *  auto result = query.data();
 * \endcode
 *
 * @note Any objects in the query well not be saved in to cache. For caching your objects use the CachedDbObjectsRequest class.
 */
template <class T>
class DBObjectsRequest final: public DBObjectSet
{
public:

    /**
     * @brief DBObjectsRequest This is default constructor for parsing packages.
     * @param pkkg This is package.
     */
    DBObjectsRequest(const Package& pkkg) {
        fromBytes(pkkg.toBytes());
    }

    /**
     * @brief DBObjectsRequest This contsrucor create a object with request the array of T objects.
     * @note If you want to get all elements from table then skip @a conditions argument or set it to empty string value.
     * @param table  This is name of database table.
     * @param conditions This is string with conditions for create sql query. If you want to get all elemts just ignore this argument.
     * @param valuesToBind This is map of the value for the reques.
     * @see DBObjectsRequest::setConditions
     */
    DBObjectsRequest(const QString& table,
                     const QString& conditions = "",
                     const QVariantMap& valuesToBind = {}):
        DBObjectSet (table) {

        _conditions = conditions;
        _condirionValues = valuesToBind;
    };

    void clear() override {
        _data.clear();
    };

    bool fromSqlRecord(const QSqlRecord &q) override {
        auto ptr = QSharedPointer<T>::create();

        if (!ptr->fromSqlRecord(q)) {
            clear();
            return false;
        }

        _data.push_back(ptr);

        return true;
    };

    QDataStream &fromStream(QDataStream &stream) override {
        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override {
        return stream;
    };

    bool isValid() const override {
        return true;
    };

    /**
     * @brief data This method return a list of getted objects.
     * @return list of const T * objects.
     */
    const QList<QSharedPointer<T>> & data() const {
        return _data;
    };

    DBVariantMap variantMap() const override {
        return T{}.variantMap();
    };

    /**
     * @brief setConditions This method sets condition value for request.
     * @param newConditions This is new condition value.
     */
    void setConditions(const QString &newConditions) {
        _conditions = newConditions;
    }

protected:

    std::pair<QString, QMap<QString, QVariant> > condition() const override {
        return {_conditions, _condirionValues};
    }

    DBObject *createDBObject() const override {
        return create<DBObjectsRequest<T>>(table(), _conditions);
    };

    QList<QSharedPointer<T>> _data;

private:
    QString _conditions;
    QVariantMap _condirionValues;


};

}
}
#endif // DBOBJECTSREQUEST_H
