/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "package.h"
#include <streambase.h>
#include <global.h>

namespace NP {

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
    unsigned short _cmd = 0;

protected:
    /**
     * @brief AbstractData
     */
    explicit AbstractData();

    /**
     * @brief fromBytes - private initialisation of object from byte array
     * @return true if all good
     */
    bool fromBytes(const QByteArray&);

    /**
     * @brief setCmd
     * @param cmd
     */
    void setCmd(unsigned short cmd);

public:

    /**
     * @brief cmd
     * @return command of package
     */
    unsigned short cmd() const;

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
    bool toPackage(Package &package, unsigned short trigeredCommand = 0) const;

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

    /**
     * @brief copyFrom this function try cast other object to this class type
     * and invoke copy operation. cmd option is ignored;
     * @param other
     * @return true if all good
     */
    virtual bool copyFrom(const AbstractData*);

    virtual ~AbstractData() override;

};


}

#define INIT_COMMAND setCmd(H_16<decltype (*this)>());


#endif // ABSTRACTDATA_H
