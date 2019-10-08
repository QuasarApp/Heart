#ifndef SERVERPROTOCOL_H
#define SERVERPROTOCOL_H

#include "serverprotocol_global.h"

#include <QVariantMap>

#define DEFAULT_SERVER "SnnakeServer"

namespace ServerProtocol {

enum Type: unsigned char {
    Responke = 0,
    Request = 1
};

enum Command: unsigned char {
    Undefined = 0x00,
    Ping = 0x01,
    State = 0x02,
    Stop = 0x03,
    Restart = 0x04,
    Unban = 0x05,
    Ban = 0x06
};

/**
 * @brief The Header struct 2 byte
 */
struct SERVERPROTOCOLSHARED_EXPORT Header {
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size: 10;
    /**
     * @brief type of package see Type
     */
    unsigned char type: 1;
    /**
     * @brief command of pacage see Command
     */
    unsigned char command: 5;

    /**
     * @brief Header default constructor
     */
    Header();

    /**
     * @brief isValid
     * @return true if header is valid
     */
    bool isValid() const;


    /**
     * @brief reset - reset all data and set for header invalid status
     */
    void reset();
};

/**
 * @brief The Package struct
 */
struct SERVERPROTOCOLSHARED_EXPORT Package {
    /**
     * @brief hdr - header of package
     */
    Header hdr;
    /**
     * @brief data - source data of package
     */
    QByteArray data;

    Package();

    /**
     * @brief isValid
     * @return true if package is valid
     */
    bool isValid() const;

    /**
     * @brief parse
     * @return Qmap of package (default key if "value")
     */
    QVariantMap parse() const;

    /**
     * @brief toBytes
     * @return bytes array of packag
     */
    QByteArray toBytes() const;

    /**
     * @brief reset - reset all data and set for package invalid status
     */
    void reset();

    bool fromMap(const QVariantMap& map);

};

}

#endif // SERVERPROTOCOL_H
