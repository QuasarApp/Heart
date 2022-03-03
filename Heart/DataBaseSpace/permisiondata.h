/*
 * Copyright (C) 2018-2022 QuasarApp.
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
    PermisionData(const QVariant& subject, const QString& objectAddress);

    friend bool operator == (const PermisionData& left, const PermisionData& right);
    unsigned int hash() const override;
    bool isValid() const override;
    bool equal(const AbstractKey *other) const override;
    QString toString() const override;

    /**
     * @brief setId This method set id of Network member.
     * @param Id Value of network member id.
     */
    void setId(const QVariant &Id);

    /**
     * @brief addressHash This method return sha256 hash of the address of database object.
     *  The hash encoded as a base64.
     * @return address of database object.
     */
    const QString &addressHash() const;

    /**
     * @brief setAddress This method set address of database object.
     * @param address This is new value of database address.
     */
    void setAddress(const DbAddress &address);

    /**
     * @brief setAddress This implementation sets sha256 hash of the address (hash must be write in base64 encoding)
     * @param addressHash This is base64 string of a sha256 hash code.
     */
    void setAddress(const QString &addressHash);

    // StreamBase interface
    const QVariant &id() const;

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    /// id  of user of node
    QVariant _id;

    /// table of target object (second part of key)
    QString _addressHash;

};

}
#endif // PERMISIONDATA_H
