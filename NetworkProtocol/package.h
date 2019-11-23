#ifndef ABSTRACTPACKAGE_H
#define ABSTRACTPACKAGE_H
#include "header.h"
#include "networkprotocol_global.h"

#include <QByteArray>

namespace NetworkProtocol {

class Abstract;
/**
 * @brief The Package struct
 */
struct NETWORKPROTOCOLSHARED_EXPORT Package {
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
    virtual QByteArray toBytes() const;

    /**
     * @brief fromBytes
     * @return bytes array to packag
     */
    virtual void fromBytes(const QByteArray &array);
    /**
     * @brief reset - reset all data and set for package invalid status
     */
    virtual void reset();

    virtual ~Package() = default;

};

}
#endif // ABSTRACTPACKAGE_H
