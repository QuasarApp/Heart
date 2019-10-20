#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "package.h"

namespace ClientProtocol {

/**
 * @brief The AbstractData class
 * all data packages inherited this class.
 */
class CLIENTPROTOCOLSHARED_EXPORT AbstractData
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
    explicit AbstractData();

    /**
     * @brief AbstractData
     * @param package
     */
    explicit AbstractData(const Package& package);

    /**
     * @brief fromBytes - private initialisation of object from byte array
     * @return true if all good
     */
    bool fromBytes(const QByteArray&);

    /**
     * @brief generateId
     * @return generate cmd function
     */
    virtual int generateId();

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
    bool toPackage(Package &package, unsigned char trigeredCommand = 0) const;

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

    virtual ~AbstractData();

    friend class testSankeServer;


};


}


#endif // ABSTRACTDATA_H
