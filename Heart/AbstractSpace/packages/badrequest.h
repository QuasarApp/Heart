/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef BADREQUEST_H
#define BADREQUEST_H

#include "abstractdata.h"

namespace QH{
namespace PKG {

/**
 * @brief The BadRequest class send response about error to client
 */
class BadRequest : public AbstractData
{
public:
    /**
     * @brief BadRequest
     * @param err This is error message.
     */
    explicit BadRequest(const QString & err = "");
    explicit BadRequest(const Package& package);

    /**
     * @brief err This method return a text of error message.
     * @return text of error message
     */
    QString err() const;

    /**
     * @brief setErr - set a error mesage.
     * @param err - message of error.
     */
    void setErr(const QString &err);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    QString _err;

};
}
}

#endif // BADREQUEST_H
