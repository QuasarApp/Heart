#ifndef CLOSECONNECTION_H
#define CLOSECONNECTION_H

#include "abstractdata.h"


namespace QH{

/**
 * @brief The CloseConnection class - This commnad is request for close connection on parent node of connection
 * This neede becouse QAbstractSocket emit error when connection closed from remote node.
 *
 * From QtDocumentation :
 * ``` If the remote host closes the connection, QAbstractSocket will emit errorOccurred(QAbstractSocket::RemoteHostClosedError), during which the socket state will still be ConnectedState, and then the disconnected() signal will be emitted.```
 */
class NETWORKPROTOCOLSHARED_EXPORT CloseConnection: public AbstractData
{
public:
    CloseConnection();
};
}
#endif // CLOSECONNECTION_H
