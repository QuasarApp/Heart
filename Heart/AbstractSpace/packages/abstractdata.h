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

namespace QH {

/**
 * @brief The AbstractData class
 * all data packages inherited this class.
 */
class NETWORKPROTOCOLSHARED_EXPORT AbstractData : public StreamBase
{
public:

    virtual ~AbstractData() override;

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
    bool toPackage(Package &package, unsigned int triggerHash = 0) const;

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

    /**
     * @brief toString - return a string implementation fo this object
     * @return string of object
     */
    virtual QString toString() const;

    /**
     * @brief prepareToSend - this method check object to valid and if an object is invalid invoke method init.
     * @return return true if the object prepared for sending.
     */
    bool prepareToSend();

    /**
     * @brief create - this is factory method for create a new object with some type that parent object.
     * @param args - list of arguments for create object
     * @return pointer toObject
     */
    template<class C, class... Args>
    C* create(Args&&... args) const {
        C* object = new C(std::forward<Args>(args)...);
        object->generateCmd();
        return object;
    }

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

    /**
     * @brief init - this method need to invoke after create object for initialize all componet of ojects.
     * @note do not invode this method on constructor of object, becose object wel be initialized not correctly.
     * @default defaul implementation of object init _com of object.
     * @return true if object initialized correctly.
     */
    virtual bool init();


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

private:
    /**
     * @brief generateCmd set cmd from class name.
     * @note call this method only after create objects. do not call in constructor of class.
     */
    void generateCmd();
    /**
     * @brief _cmd - unique id of class using in Header of package for identification.
     */
    unsigned short _cmd = 0;
};


}

#endif // ABSTRACTDATA_H
