/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef HOSTADDRESS_H
#define HOSTADDRESS_H
#include "heart_global.h"

#include <QHostAddress>
#include "config.h"

namespace QH {

/**
 * @brief The Host Address class this is wrapper of QHostAddress. Contains the NetworkAddress and network port.
 */
class HEARTSHARED_EXPORT HostAddress: public QHostAddress
{
public:
    explicit HostAddress();
    explicit HostAddress(const QHostAddress& other, int port = DEFAULT_PORT);
    explicit HostAddress(const QString& other, int port = DEFAULT_PORT);
    HostAddress(const SpecialAddress& other, int port = DEFAULT_PORT);

    HostAddress(const HostAddress& other);


    /**
     * @brief The port method return port of node.
     * @return port of node.
     */
    unsigned short port() const;

    /**
     * @brief setPort This method set new port value.
     * @param port This is new value of port.
     */
    void setPort(unsigned short port);

    friend QDataStream& operator << (QDataStream& stream, const HostAddress& address);
    friend QDataStream& operator >> (QDataStream& stream, HostAddress& address);

    friend bool& operator == (QDataStream& stream, HostAddress& address);
    friend bool& operator != (QDataStream& stream, HostAddress& address);

    HostAddress& operator = (const HostAddress& right);;
    /**
     * @brief isIP This method return true if the @a address is valid.
     * @brief address This is tring value of the network ip address.
     * @return true if the @a address is valid.
     */
    static bool isIP(const QString& address);

    /**
     * @brief isValid This method return true if the current network address is valid.
     * @return true if the current network address is valid.
     */
    bool isValid() const;

    /**
     * @brief toString this method convert the Host Address value to string value.
     * @return string value of the HostAddress object.
     */
    QString toString() const;

    /**
     * @brief toBytes This method convert a network address to a byte array.
     * @return byte array implementation of this object.
     */
    QByteArray toBytes() const;

    /**
     * @brief fromBytes This method applay @a array obejct. (conver a byte array to a newtwork address object)
     * @param array this is input array.
     * @return true if array applayed successfull
     */
    bool fromBytes(const QByteArray& array);

private:
    unsigned short _port = 0;

};

uint qHash(const HostAddress& address);
}

Q_DECLARE_METATYPE(QH::HostAddress);

#endif // HOSTADDRESS_H
