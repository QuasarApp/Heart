/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef GETAVAILABLEDATA_H
#define GETAVAILABLEDATA_H

#include "abstractdata.h"

namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT AvailableData : public AbstractData
{
public:
    AvailableData();
    AvailableData(const Package& pkg);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    // AbstractData interface
    bool isValid() const;

    QHash<QString, QList<int> > data() const;
    void setData(const QHash<QString, QList<int> > &data);

    QList<int>& operator[](const QString& key);

private:
    QHash<QString, QList<int>> _data;



};

}
#endif // GETAVAILABLEDATA_H
