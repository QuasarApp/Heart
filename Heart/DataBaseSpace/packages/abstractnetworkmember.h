/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef ABSTRACTNETWORKMEMBER_H
#define ABSTRACTNETWORKMEMBER_H

#include "dbobject.h"


namespace QH {
namespace PKG {

/**
 * @brief The AbstractNetworkMember class is structure of network member.
 *  Usually it is base informations of node or client.
 */
class HEARTSHARED_EXPORT AbstractNetworkMember: public DBObject
{
public:
    AbstractNetworkMember();
    AbstractNetworkMember(const Package& pkg);
    AbstractNetworkMember(const QVariant &id);

    // DBObject interface
    bool fromSqlRecord(const QSqlRecord &q) override;

    /**
     * @brief authenticationData is data array requirement for authentication node or client on the network.
     * @return The byte Array of authentication data.
     */
    QByteArray authenticationData() const;

    /**
     * @brief setAuthenticationData This method set new array of authentication data.
     * @param data This is new data of authentication data.
     */
    void setAuthenticationData(const QByteArray &data);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;
    DBVariantMap variantMap() const override;

    /**
     * @brief trust This is trust level of current Network member.
     * IF  the trust level less then 0 node or client will be blocked (banned).
     * @return current trust level of this node.
     */
    int trust() const;

    /**
     * @brief changeTrust This method change a current trust level.
     * @param diff This is a diff of current value. (currentTrust += diff;)
     */
    void changeTrust(int diff);

    /**
     * @brief setTrust This method set a new value of trust  level of the node.
     * @param trust This is a new value.
     */
    void setTrust(int trust);


protected:

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    QString primaryKey() const override;

private:
    QByteArray _authenticationData;
    int _trust;


};
}
}
#endif // ABSTRACTNETWORKMEMBER_H
