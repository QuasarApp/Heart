/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef BADREQUEST_H
#define BADREQUEST_H

#include "abstractdata.h"

namespace NP{

/**
 * @brief The BadRequest class
 */
class BadRequest : public AbstractData
{
public:
    explicit BadRequest(const QString & err = "");
    explicit BadRequest(const Package& package);

    QString err() const;
    void setErr(const QString &err);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    QString _err;

};

}

#endif // BADREQUEST_H
