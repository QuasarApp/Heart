/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NODEOBJECT_H
#define NODEOBJECT_H

#include "dbobject.h"


namespace NP {

/**
 * @brief The NodeObject class - database structure of node
 */
class NETWORKPROTOCOLSHARED_EXPORT NodeObject: public DBObject
{
public:
    NodeObject();
    NodeObject(const Package& pkg);
    NodeObject(const BaseId& id);

    // DBObject interface
    DBObject *factory() const override;
    PrepareResult prepareSaveQuery(QSqlQuery &q) const override;
    bool fromSqlRecord(const QSqlRecord &q) override;

    QByteArray publickKey() const;
    void setPublickKey(const QByteArray &publickKey);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    int trust() const;
    void changeTrust(int diff);
    void setTrust(int trust);

protected:

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QByteArray _publickKey;
    int _trust;


};
}
#endif // NODEOBJECT_H
