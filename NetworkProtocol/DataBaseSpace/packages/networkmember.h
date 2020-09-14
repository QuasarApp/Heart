/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NETWORKMEMBER_H
#define NETWORKMEMBER_H

#include "dbobject.h"


namespace NP {

/**
 * @brief The NodeObject class - database structure of node
 */
class NETWORKPROTOCOLSHARED_EXPORT NetworkMember: public DBObject
{
public:
    NetworkMember();
    NetworkMember(const Package& pkg);
    NetworkMember(const BaseId& id);

    // DBObject interface
    DBObject *factory() const override;
    PrepareResult prepareSaveQuery(QSqlQuery &q) const override;
    bool fromSqlRecord(const QSqlRecord &q) override;

    QByteArray authenticationData() const;
    void setAuthenticationData(const QByteArray &publickKey);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    QPair<QString, QString> altarnativeKey() const override;

    int trust() const;
    void changeTrust(int diff);
    void setTrust(int trust);


protected:

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    BaseId generateId() const override;

private:
    QByteArray _authenticationData;
    int _trust;
};
}
#endif // NETWORKMEMBER_H
