/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef AVAILABLEDATAREQUEST_H
#define AVAILABLEDATAREQUEST_H

#include "availabledata.h"

#include <accesstoken.h>


namespace NP {

class NETWORKPROTOCOLSHARED_EXPORT AvailableDataRequest :public AvailableData {
public:
    AvailableDataRequest();
    AvailableDataRequest(const Package &pkg);

    void setData(const QHash<QString, QList<int> > &data) = delete;
    AccessToken token() const;
    void setToken(const AccessToken &token);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:

    AccessToken _token;


};

}

#endif // AVAILABLEDATAREQUEST_H
