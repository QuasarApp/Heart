/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef GETAVAILABLEDATA_H
#define GETAVAILABLEDATA_H

#include "dbobject.h"

namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT AvailableData : public DBObject
{
public:
    AvailableData();
    AvailableData(const Package& pkg);

    QList<DbAddress> data() const;
    void setData(const QList<DbAddress> &data);

    DbAddress& operator[](int key);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    // DBObject interface
    void clear() override;
    DBObject *factory() const override;
    PrepareResult prepareSelectQuery(QSqlQuery &q) const override;
    PrepareResult prepareSaveQuery(QSqlQuery &q) const override;
    PrepareResult prepareRemoveQuery(QSqlQuery &q) const override;

    bool isCached() const override;
    bool isBundle() const override;
    bool fromSqlRecord(const QSqlRecord &q) override;

protected:
    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QList<DbAddress> _data;

};

}
#endif // GETAVAILABLEDATA_H
