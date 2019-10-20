#ifndef ABSTRACTPACKAGE_H
#define ABSTRACTPACKAGE_H
#include "abstractheader.h"
#include "clientprotocol_global.h"

#include <QByteArray>

namespace ClientProtocol {

class Abstract;
/**
 * @brief The Package struct
 */
struct CLIENTPROTOCOLSHARED_EXPORT AbstractPackage {
    /**
     * @brief hdr - header of package
     */
    AbstractHeader hdr;
    /**
     * @brief data - source data of package
     */
    QByteArray data;

    AbstractPackage();

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

    virtual ~AbstractPackage() = default;

};

}
#endif // ABSTRACTPACKAGE_H
