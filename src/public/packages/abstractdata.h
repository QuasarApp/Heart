/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the GPLv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "distversion.h"
#include "humanreadableobject.h"
#include "package.h"
#include <QSharedPointer>
#include <streambase.h>
#include <crc/crchash.h>

/**
 * @brief PROTOCKOL_VERSION_COMMAND is command for exchange versions number betwin nodes.
 */
#define PROTOCKOL_VERSION_COMMAND 0xFFFF

/**
 * @brief PROTOCKOL_VERSION_RECEIVED_COMMAND is command for check delivered version.
 */
#define PROTOCKOL_VERSION_RECEIVED_COMMAND PROTOCKOL_VERSION_COMMAND - 1

/**
 * @brief QH_PACKAGE This macross prepare data to send and create a global id for package.
 * For get global id use the cmd method.
 * For get quick access for global command use the ClassName::command() method. This method is static.
 * @arg S This is unique id of the pacakge. Shold be some on all your network devices.

*/
#define QH_PACKAGE(S) \
   public: \
    static unsigned short command(){\
        QByteArray ba = QString(S).toLocal8Bit();\
        return qa_common::hash16(ba.data(), ba.size());\
    } \
    static QString commandText(){return S;} \
    unsigned short cmd() const override {return command();} \
\
    QString cmdString() const override {return S;} \
   private:

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
    QH_PACKAGE_AUTO_VER(MyPackage, 1)
public:
    MyPackage();

    // your data for for server of client
    std::string _data = "";

protected:
    // StreamBase interface override this methods for serialization your package
    QDataStream &fromStream(QDataStream &stream) {
        stream >> _data;
        return stream;
    }
    QDataStream &toStream(QDataStream &stream) const {
        stream << _data;
        return stream;
    }

};
 *  \endcode
 * This is simple implementation of package for transport text data.
 * You need to override 2 serialization methods (fromStream and toStream).

 * @see AbstractNode
 */
class HEARTSHARED_EXPORT AbstractData : public StreamBase, public QuasarAppUtils::iHRO
{
public:

    ~AbstractData() override;

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
     * @param package This is return value of Package class.
     * @param reqVersion This is required version. This method create package of the needed version.
     * @param triggerHash This is hash of the package the current class is responding to.
     * @return True if convert to package finished successful.
     */
    virtual bool toPackage(Package &package, const DistVersion &reqVersion, unsigned int triggerHash = 0) const;

    /**
     * @brief isValid This method check current object to valid.
     * @return True if class isValid.
     */
    virtual bool isValid() const;

    /**
     * @brief toString - Return a string implementation for this object.
     * @return String of object.
     */
    QString toString() const override;

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

};


}
}

#endif // ABSTRACTDATA_H
