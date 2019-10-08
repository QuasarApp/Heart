#ifndef CLIENTPROTOCOL_H
#define CLIENTPROTOCOL_H

#include "clientprotocol_global.h"

#include <QVariantMap>
#include "config.h"


namespace ClientProtocol {


bool CLIENTPROTOCOLSHARED_EXPORT initClientProtockol();

/**
 * @brief The Header struct 4 byte
 */
#pragma pack(push, 1)
struct CLIENTPROTOCOLSHARED_EXPORT Header {
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size: 12;

    /**
     * @brief command of pacage
     */
    quint8 command: 5;

    /**
     * @brief command of pacage see Command (rquest from client)
     * the server should write to which command it responds
     */
    quint8 triggerCommnad: 5;

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
#pragma pack(pop)

/**
 * @brief The Package struct
 */
struct CLIENTPROTOCOLSHARED_EXPORT Package {
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
    virtual bool isValid() const;

    /**
     * @brief toBytes
     * @return bytes array of packag
     */
    QByteArray toBytes() const;

    /**
     * @brief reset - reset all data and set for package invalid status
     */
    void reset();

    virtual ~Package() = default;

};

}

#endif // CLIENTPROTOCOL_H
