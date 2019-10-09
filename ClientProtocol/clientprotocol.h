#ifndef CLIENTPROTOCOL_H
#define CLIENTPROTOCOL_H

#include "clientprotocol_global.h"

#include <QVariantMap>
#include "config.h"


namespace ClientProtocol {

/**
 * @brief The Header struct 4 byte
 */
#pragma pack(push, 1)
struct CLIENTPROTOCOLSHARED_EXPORT BaseHeader {
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size;

    /**
     * @brief command of pacage
     */
    quint8 command;

    /**
     * @brief command of pacage see Command (rquest from client)
     * the server should write to which command it responds
     */
    quint8 triggerCommnad;

    /**
     * @brief Header default constructor
     */
    BaseHeader();
    virtual ~BaseHeader() = default;

    /**
     * @brief isValid
     * @return true if header is valid
     */
    virtual bool isValid() const;


    /**
     * @brief reset - reset all data and set for header invalid status
     */
    virtual void reset();
};
#pragma pack(pop)

/**
 * @brief The Package struct
 */
struct CLIENTPROTOCOLSHARED_EXPORT BasePackage {
    /**
     * @brief hdr - header of package
     */
    BaseHeader hdr;
    /**
     * @brief data - source data of package
     */
    QByteArray data;

    BasePackage();

    /**
     * @brief isValid
     * @return true if package is valid
     */
    virtual bool isValid() const;

    /**
     * @brief toBytes
     * @return bytes array of packag
     */
    virtual QByteArray toBytes() const;

    /**
     * @brief reset - reset all data and set for package invalid status
     */
    virtual void reset();

    virtual ~BasePackage() = default;

};

}

#endif // CLIENTPROTOCOL_H
