/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "package.h"
#include <streambase.h>


/**
 * @brief QH_PACKAGE This macross prepare data to send and create a global id for package. For get global id use the cmd method.
 * For get quick access for global command use the ClassName::command() method. This method is static.
*/
#define QH_PACKAGE(X, S) \
   public: \
    static unsigned short command(){return qHash(QString(S)) % 0xFFFF;} \
    static QString commandText(){return S;} \
    unsigned short cmd() const override {return X::command();} \
    QString cmdString() const override {return X::commandText();} \
   protected: \
    unsigned int localCode() const override {return typeid(X).hash_code();} \
    \
   private:


#define QH_PACKAGE_AUTO(X) QH_PACKAGE(X,#X)

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
    QH_PACKAGE(MyPackage, "MyPackage")
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
 *
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
 * @see AbstractNode
 */
class HEARTSHARED_EXPORT AbstractData : public StreamBase
{
public:

    virtual ~AbstractData() override;

    /**
     * @brief cmd - This is command of this object, (for generate cmd use macross QH_PACKAGE)
     * @note Use the QH_PACKAGE macross for implement this method.
     * @return global command of package.
     * @see QH_PACKAGE
     */
    virtual unsigned short cmd() const = 0;

    /**
     * @brief cmd - This is command string of this object, (for generate cmd use macross QH_PACKAGE)
     * @note Use the QH_PACKAGE macross for implement this method.
     * @return global command of package.
     * @see QH_PACKAGE
     */
    virtual QString cmdString() const = 0;

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
     * @brief create - This is factory method for create a new object.
     * @param args - List of arguments for create object.
     * @return Pointer to created object.
     */
    template<class C, class... Args>
    C* create(Args&&... args) const {
        C* object = new C(std::forward<Args>(args)...);
        return object;
    }

    /**
     * @brief fromPakcage - This method initialize the current object from a package class object.
     * @param pkg - Input package value.
     */
    void fromPakcage(const Package& pkg);

    /**
     * @brief command This static method that return glaball code of this object.
     * @note This method generated automaticaly in the QH_PACKAGE macross.
     * @return global code
     * @see QH_PACKAGE
     */
    static unsigned int command(){return 0;};

    /**
     * @brief commandText This method return text of package command
     * @return text of pacakge command
     */
    static QString commandText(){return "NULL";};

protected:
    /**
     * @brief AbstractData - Base constructor of this object.
     */
    explicit AbstractData();

    /**
     * @brief localCode This method return local code
     * @return local command of this class. used for check QH_PACKAGE macro before send pacakge.
     */
    virtual unsigned int localCode() const = 0;

    QDataStream& fromStream(QDataStream& stream) override;
    QDataStream& toStream(QDataStream& stream) const override;

private:
    /**
     * @brief checkCmd This method check QH_PACKAGE macross.
     * @return true if the QH_PACKAGE macross is enabled else fal.
     */
    bool checkCmd() const;;

};


}
}

#endif // ABSTRACTDATA_H
