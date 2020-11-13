/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DBADDRESS_H
#define DBADDRESS_H

#include "streambase.h"
#include "basedefines.h"

namespace QH {

/**
 * @brief The DbAddress class uset to work with database addresses. Database Address it is structure with 2 values.
 * \code
 * {
 *  QString _table;  // this is name of table of object.
    QVariant _id;     // this is id of object.
 * }
 * \endcode
 */
class HEARTSHARED_EXPORT DbAddress : public StreamBase {

public:

    DbAddress() = default;

    /**
     * @brief DbAddress this constructo initialize a default database address.
     * @param table This is table name in database.
     * @param primaryKey This is primary key.
     * @param id This is id of object in table.
     */
    DbAddress(const QString& table,  const QVariant& id);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    /**
     * @brief toString This method return a string implementation fo this object
     * @return string of object
     */
    QString toString() const;

    friend bool operator== (const DbAddress& left, const DbAddress& other);
    friend bool operator!= (const DbAddress& left, const DbAddress& other);

    /**
     * @brief isValid This method check object for valid.
     * @return true if object is valid.
     */
    bool isValid() const;

    /**
     * @brief table This method return table name.
     * @return table name of object.
     */
    const QString& table() const;

    /**
     * @brief setTable This method set new table name of address.
     * @param table new value of table name.
     */
    void setTable(const QString &table);

    /**
     * @brief id This method return id of object in table.
     * @return id of object.
     */
    const QVariant &id() const;

    /**
     * @brief setId This method set id for this address.
     * @param id tgis is new value of objects id.
     */
    void setId(const QVariant &id);

private:

    QString _table;
    QVariant _value;
};

/**
 * @brief qHash This functions cals int 32 hash of address.
 * @param address This is input address.
 * @return unsigned int 32 hash value.
 */
qint64 qHash(const DbAddress& address);

}


#endif // DBADDRESS_H
