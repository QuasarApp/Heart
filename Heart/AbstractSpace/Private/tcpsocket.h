/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <QTcpSocket>

namespace QH {

/**
 * @brief The AbstractSocket class This class is wraper of the QAbstract socket with slot implementation of the disconnect from host method.
 */
class TcpSocket: public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject *parent);
public slots:

    /**
     * @brief handleDisckonnetFromHost This is slot wrapper of the disconectFromHost method of QAbstractSocket class.
     */
    void handleDisckonnetFromHost();

};

}
#endif // ABSTRACTSOCKET_H
