#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "clientprotocol.h"

namespace ClientProtocol {

namespace Data {

/**
 * @brief The AbstractData class
 * all data packages inherited this class.
 */
class CLIENTPROTOCOLSHARED_EXPORT Abstract
{
private:
    /**
     * @brief _cmd - unique id of class using in Header of package for identification.
     */
    unsigned char _cmd = 0;

protected:
    /**
     * @brief AbstractData
     */
    explicit Abstract();

    /**
     * @brief AbstractData
     * @param package
     */
    explicit Abstract(const BasePackage& package);

    /**
     * @brief fromBytes - private initialisation of object from byte array
     * @return true if all good
     */
    bool fromBytes(const QByteArray&);

public:

    /**
     * @brief cmd
     * @return command of package
     */
    unsigned char cmd() const;

    /**
     * @brief toBytes
     * @return byte array for package
     */
    QByteArray toBytes() const;


    /**
     * @brief toPackage
     * @param package - return value
     * @param trigeredCommand - old cmd
     * @return retorn package object created from this object.
     */
    bool toPackage(BasePackage &package, unsigned char trigeredCommand = 0) const;

    /**
     * @brief fromStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& fromStream(QDataStream& stream);

    /**
     * @brief toStream
     * @param stream
     * @return stream
     */
    virtual QDataStream& toStream(QDataStream& stream) const;

    /**
     * @brief isValid
     * @return true if class isValid
     */
    virtual bool isValid() const;

    virtual ~Abstract();


};
}

}


#endif // ABSTRACTDATA_H
