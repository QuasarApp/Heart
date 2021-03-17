/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "socketfactory.h"

#include <abstractnode.h>
namespace QH {

SocketFactory::SocketFactory(AbstractNode *nodeInstance):
    Async(nodeInstance) {

    assert(nodeInstance && "The SocketFactory must be initialize with the mail node object.");

    _abstractNodeInstance = nodeInstance;
}

bool SocketFactory::registerSocket(QAbstractSocket *socket,
                                   const HostAddress *clientAddress) {

    return asyncLauncher(std::bind(&SocketFactory::registerSocketPrivete,
                                   this, socket, clientAddress), false);

}

bool SocketFactory::registerSocketPrivete(QAbstractSocket *socket,
                                          const HostAddress *clientAddress) {
    return _abstractNodeInstance->registerSocket(socket, clientAddress);
}

}
