/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef CLOSECONNECTION_H
#define CLOSECONNECTION_H

#include "abstractdata.h"


namespace QH{
namespace PKG {

/**
 * @brief The CloseConnection class - This commanad is request for close connection on parent node of connection.
 * This is necessary because QAbstractSocket emit error when connection closed from remote node.
 *
 * From QtDocumentation:
 *
 *
 *  If the remote host closes the connection, QAbstractSocket will emit errorOccurred(QAbstractSocket::RemoteHostClosedError), during which the socket state will still be ConnectedState, and then the disconnected() signal will be emitted.
 *
 */
class HEARTSHARED_EXPORT CloseConnection: public AbstractData
{
    QH_PACKAGE(CloseConnection, "CloseConnection")

public:
    CloseConnection();

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
};
}
}
#endif // CLOSECONNECTION_H
