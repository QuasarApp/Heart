/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef PERMISIONDATA_H
#define PERMISIONDATA_H

#include <QString>
#include <abstractkey.h>
#include "dbaddress.h"

namespace QH {

/**
 * @brief The PermisionData class stores one correspondence to a network member and an object in the database.
 * Network member - database address.
 */
class HEARTSHARED_EXPORT PermisionData: public AbstractKey, public StreamBase {

public:

    PermisionData() = default;
    PermisionData(const QVariant& subject, const DbAddress& objcet);

    friend bool operator == (const PermisionData& left, const PermisionData& right);
    unsigned int hash() const override;
    bool isValid() const override;
    bool equal(const AbstractKey *other) const override;
    QString toString() const override;

    /**
     * @brief setId This method set id of Network member.
     * @param Id This
     */
    void setId(const QVariant &Id);

    /**
     * @brief address This method return address of database object.
     * @return address of database object.
     */
    const DbAddress &address() const;

    /**
     * @brief setAddress This method set address of database object.
     * @param address This is new valueof database address.
     */
    void setAddress(const DbAddress &address);

    // StreamBase interface
    const QVariant &id() const;

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    /// id  of user of node
    QVariant _id;

    /// table of target object (second part of key)
    DbAddress _address;
};

}
#endif // PERMISIONDATA_H
