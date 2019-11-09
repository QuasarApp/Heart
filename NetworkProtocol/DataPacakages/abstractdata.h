#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "package.h"

#include <streambase.h>

namespace NetworkProtocol {

/**
 * @brief The AbstractData class
 * all data packages inherited this class.
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractData : public StreamBase
{
private:
    /**
     * @brief _cmd - unique id of class using in Header of package for identification.
     */
    unsigned int _cmd = 0;

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
    virtual unsigned int generateId();

public:

    /**
     * @brief cmd
     * @return command of package
     */
    unsigned int cmd() const;

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
    bool toPackage(Package &package, unsigned int trigeredCommand = 0) const;

    /**
     * @brief fromStream
     * @param stream
     * @return stream
     */
    QDataStream& fromStream(QDataStream& stream) override;

    /**
     * @brief toStream
     * @param stream
     * @return stream
     */
    QDataStream& toStream(QDataStream& stream) const override;
    /**
     * @brief isValid
     * @return true if class isValid
     */
    virtual bool isValid() const;

    virtual ~AbstractData() override;

};


}


#endif // ABSTRACTDATA_H
