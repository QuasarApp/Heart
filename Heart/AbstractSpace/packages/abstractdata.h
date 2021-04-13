/*
 * Copyright (C) 2018-2021 QuasarApp.
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
namespace PKG {

/**
 * @brief The AbstractData class is provide base functions for transport data by network
 *  For create you own package you need to inherited from this class.
 *  Simple Example:
 *
 *  \code{cpp}
 * class MyPackage: public QH::AbstractData
{
public:
    MyPackage();

    // your data for for server of client
    std::string _data = "";

protected:
    // StreamBase interface override this methods for serialization your package
    QDataStream &fromStream(QDataStream &stream) {
        AbstractData::fromStream(stream);
        stream >> _data;
        return stream;
    }
    QDataStream &toStream(QDataStream &stream) const {
        AbstractData::toStream(stream);
        stream << _data;
        return stream;
    }

};
 *  \endcode
 * This is simple implementation of package for transport text data.
 * You need to override 2 serialization methods (fromStream and toStream).
 * For add the copying functionality of you class, you need to override the copyFrom  method.
 * \code {cpp}
 * class MyPackage: public QH::AbstractData
{
    ...
    bool copyFrom(const AbstractData *other) {
        if (!AbstractData::copyFrom(other))
            return false;

        auto otherObject = dynamic_cast<const MyPackage*>(other);
        if (!otherObject)
            return false;

        this->_data = otherObject->_data;
        return true;
    };

    // your data for for server of client
    std::string _data = "";
    ...
};
 * \endcode
 * This method copyFrom supports copying between parent and child classes.
 * Consider the situation:
 * Classes A > B > C
\code{cpp}
A * a = new A;
A * b = new B;
A * c = new C;
\endcode

 * The situation when the classes have the same type.
 * In this case, all object fields will be copied.

\code{cpp}
b.copyFrom (b1) // true
c.copyFrom (c1) // true
\endcode

 * The situation when the class Requesting data to be copied is a child of the class that is requesting data to be copied.
\code{cpp}
b.copyFrom (a) // false
c.copyFrom (a) // false
\endcode

* In this case, copying will be partially performed and the result of calling the copyFrom method will be false, since the data has not been copied for all members of the child class.
* The situation when the class Requesting data to be copied is the parent of the class from which data to be copied is requested.
* In this case, all possible data will be copied and returned true.

\code{cpp}
a.copyFrom (b) // true
a.copyFrom (c) // true
\endcode
 * That is why it is important that the implementation of the copyFrom method is exactly the same as shown in the example.
 * If the implementation of this method differs from the example, the data will not be copied correctly.
 */
class HEARTSHARED_EXPORT AbstractData : public StreamBase
{
public:

    virtual ~AbstractData() override;

    /**
     * @brief cmd - This is command of this object, (for generate cmd use macross H16<ClassName>)
     * @return Command of package.
     */
    unsigned short cmd() const;

    /**
     * @brief toPackage This method convert this class object to the package.
     *  For more info see Package class.
     * @param package  This is return value of Package class.
     * @param triggerHash This is hash of the package the current class is responding to.
     * @return True if convert to package finished successful.
     */
    bool toPackage(Package &package, unsigned int triggerHash = 0) const;

    /**
     * @brief isValid This method check current object to valid.
     * @return True if class isValid.
     */
    virtual bool isValid() const;

    /**
     * @brief copyFrom This method copyFrom supports copying between parent and child classes.
     *  @note The implementation of this method must be like a example:
     *   \code
         bool copyFrom(const AbstractData *other) {
            if (!AbstractData::copyFrom(other))
                return false;

            auto otherObject = dynamic_cast<const MyPackage*>(other);
            if (!otherObject)
                return false;

            this->_data = otherObject->_data;
            return true;
        };
     *   \endcode
     * @param other - Pointer to anther object.
     * @return True all members of current object is updated.
     */
    virtual bool copyFrom(const AbstractData* other);

    /**
     * @brief toString - Return a string implementation for this object.
     * @return String of object.
     */
    virtual QString toString() const;

    /**
     * @brief prepareToSend - This method check object to valid and if an object is invalid invoke method init.
     * @return Return true if the object prepared for sending.
     */
    bool prepareToSend();

    /**
     * @brief create - This is factory method for create a new object.
     * @param args - List of arguments for create object.
     * @return Pointer to created object.
     */
    template<class C, class... Args>
    C* create(Args&&... args) const {
        C* object = new C(std::forward<Args>(args)...);
        object->generateCmd();
        return object;
    }

    /**
     * @brief fromPakcage - This method initialize the current object from a package class object.
     * @param pkg - Input package value.
     */
    void fromPakcage(const Package& pkg);

protected:
    /**
     * @brief AbstractData - Base constructor of this object.
     */
    explicit AbstractData();


    /**
     * @brief setCmd - Set the new value command of object.
     * @param cmd - New value.
     */
    void setCmd(unsigned short cmd);

    /**
     * @brief Init This method need to invoke after create object for initialize all component of objects.
     * @note Do not invade this method on constructor of object, because object will be initialized not correctly.
     *  By default implementation of object init command.
     * @return True if object initialized correctly.
     */
    virtual bool init();

    QDataStream& fromStream(QDataStream& stream) override;
    QDataStream& toStream(QDataStream& stream) const override;

private:
    /**
     * @brief generateCmd Generate command from name of this class object.
     * @note Call this method only after create objects. Do not call in constructor of class.
     * @return Command of object.
     */
    unsigned short generateCmd() const;


    /**
     * @brief initCmd Set cmd from class name.
     * @note Call this method only after create objects. Do not call in constructor of class.
     */
    void initCmd();

    /**
     * @brief cmd - Unique id of class using in Header of package for identification.
     */
    unsigned short _cmd = 0;
};


}
}

#endif // ABSTRACTDATA_H
