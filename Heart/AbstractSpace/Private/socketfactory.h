/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SOCKETFACTORY_H
#define SOCKETFACTORY_H

#include <async.h>
class QAbstractSocket;

namespace QH {

To - Do // Remove This class and create a new general class for moving invokes into another threads.

class HostAddress;
class AbstractNode;

/**
 * @brief The SocketFactory class prepare socket opbjects.
 * This class is thread wraper for the registerSocket socket method of the AbstractNode class.
 * The registerSocket method of this class just invoke the AbstractNode::registerSocket method on new thread.
 */
class SocketFactory : public Async
{
    Q_OBJECT
public:
    SocketFactory(AbstractNode* nodeInstance);

    /**
     * @brief registerSocket This method registration new socket object.
     * @param socket This is incomming socket pointer.
     * @param address This is host address of socket. By default is nullptr.
     *  Set this value for nodes created on this host.
     * @return return true if the scoeket has been registered successful.
     */
    bool registerSocket(QAbstractSocket *socket, const HostAddress* clientAddress = nullptr);

private:
    bool registerSocketPrivete(QAbstractSocket *socket, const HostAddress* clientAddress);

    AbstractNode *_abstractNodeInstance = nullptr;
};
}
#endif // SOCKETFACTORY_H
