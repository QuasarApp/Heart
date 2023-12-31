/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBOBJECTSREQUESTWITHSTREAM_H
#define DBOBJECTSREQUESTWITHSTREAM_H

#include "dbobjectsrequest.h"

#include <QDataStream>


namespace QH {
namespace PKG {

/**
 * @brief The DBObjectsRequestWithStream class is template class some as DBObjectsRequest but with implementation for the StreamBase::fromStream and StreamBase::toStream methods.
 *
 * @see DBObjectsRequest
 * @see DBObjectSet
 */
template <class T>
class DBObjectsRequestWithStream final: public DBObjectsRequest<T>
{
public:

    /**
     * @brief DBObjectsRequest This is default constructor for parsing packages.
     * @param pkkg This is package.
     */
    DBObjectsRequestWithStream(const Package& pkkg):DBObjectsRequest<T>(pkkg) {}

    /**
     * @brief DBObjectsRequest This contsrucor create a object with request the array of T objects.
     * @note If you want to get all elements from table then skip @a conditions argument or set it to empty string value.
     * @param table  This is name of database table.
     * @param conditions This is string with conditions for create sql query. If you want to get all elemts just ignore this argument.
     * @see DBObjectsRequest::setConditions
     */
    DBObjectsRequestWithStream(const QString& table,
                     const QString& conditions = "" ):
        DBObjectsRequest<T> (table, conditions) { };

protected:

    QDataStream &fromStream(QDataStream &stream) override {
        DBObjectSet::fromStream(stream);

        int size = 0;
        stream >> size;

        for (int i = 0; i < size; ++i) {
            auto ptr = QSharedPointer<T>::create();

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

};

}
}
#endif // DBOBJECTSREQUESTWITHSTREAM_H
