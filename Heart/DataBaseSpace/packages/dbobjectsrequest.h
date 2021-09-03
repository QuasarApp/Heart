/*
 * Copyright (C) 2018-2021 QuasarApp.
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
     * @param table  This is name of database table.
     * @param conditions This is string with conditions for create sql query.
     */
    DBObjectsRequest(const QString& table,
                     const QString& conditions):
        DBObjectSet (table) {

        _conditions = conditions;
    };

    void clear() override {
        for (auto object: _data) {
            delete object;
        }
        _data.clear();
    };

    bool fromSqlRecord(const QSqlRecord &q) override {
        T *ptr = new T;

        if (!ptr->fromSqlRecord(q)) {
            clear();
            delete ptr;
            return false;
        }

        _data.push_back(ptr);

        return true;
    };

    bool isValid() const override {
        return _conditions.size();
    };

    bool copyFrom(const AbstractData *) override {
        return false;
    };

    /**
     * @brief data This method return a list of getted objects.
     * @return list of const T * objects.
     */
    const QList<T *>& data() const {
        return _data;;
    };

    DBVariantMap variantMap() const override {
        return T{}.variantMap();
    };
protected:

    QDataStream &fromStream(QDataStream &stream) override {
        DBObjectSet::fromStream(stream);

        clear();
        int size = 0;
        stream >> size;

        for (int i = 0; i < size; ++i) {
            T *ptr = new T;
            stream >> *ptr;

            _data.push_back(ptr);
        }

        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override {
        DBObjectSet::toStream(stream);

        stream << static_cast<int>(_data.size());

        for (auto object: _data) {
            stream << *object;
        }

        return stream;
    };

    QString condition() const override {
        return _conditions;
    }
    DBObject *createDBObject() const override {
        return create<DBObjectsRequest<T>>(tableName(), _conditions);
    };

private:
    QString _conditions;
    QList<T*> _data;

};

}
}
#endif // DBOBJECTSREQUEST_H
