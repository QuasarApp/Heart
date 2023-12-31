/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef BADREQUEST_H
#define BADREQUEST_H

#include "abstractdata.h"
#include "abstracterrorcodes.h"

namespace QH{
namespace PKG {

/**
 * @brief The ErrorData struct is simple structure for contains data of the error.
 */
struct ErrorData {
    /**
     * @brief code This is code of error. By Default this member equals EccorCodes::UnknownError.
     */
    unsigned char code = 0;
    /**
     * @brief msg This is message of error.
     */
    QString msg;
};

/**
 * @brief The BadRequest class send response about error to client
 */
class HEARTSHARED_EXPORT BadRequest : public AbstractData
{
    QH_PACKAGE("BadRequest")
public:

    /**
     * @brief BadRequest
     * @param errocode This is error code.
     * @param err This is error message.
     */
    explicit BadRequest(unsigned char errocode = ErrorCodes::InvalidRequest, const QString & err = "");

    /**
     * @brief BadRequest Init BadRequest from the ErrorData struct.
     * @param data This is error data. for more information see the ErrorData struct.
     */
    explicit BadRequest(const ErrorData& data);
    explicit BadRequest(const Package& package);

    /**
     * @brief err This method return a text of error message.
     * @return Text of error message.
     */
    QString err() const;

    /**
     * @brief setErr - Set a error message.
     * @param err - Message of error.
     */
    void setErr(const QString &err);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief errCode This method return code of error.
     * @return Code of error.
     */
    unsigned char errCode() const;

    /**
     * @brief setErrCode This method set error code.
     * @param code This is new value of error.
     */
    void setErrCode(unsigned char code);

private:
    QString _err;
    unsigned char _errCode;

};
}
}

#endif // BADREQUEST_H
