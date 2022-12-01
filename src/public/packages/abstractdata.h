/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H
#include "humanreadableobject.h"
#include "package.h"
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
 * @brief QH_PACKAGE This macross prepare data to send and create a global id for package. For get global id use the cmd method.
 * For get quick access for global command use the ClassName::command() method. This method is static.
*/
#define QH_PACKAGE(X, S) \
   public: \
    static unsigned short commandOld(){return qHash(QString(S)) % 0xFFFF;} \
    static unsigned short command(){return common::Hash16(S, strlen(S));} \
    static QString commandText(){return S;} \
    unsigned short cmd() const override {return X::command();} \
    unsigned short cmdOld() const override {return X::commandOld();} \
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
     * @param package  This is return value of Package class.
     * @param triggerHash This is hash of the package the current class is responding to.
     * @return True if convert to package finished successful.
     */
    bool toPackage(Package &package, unsigned int triggerHash = 0) const;

    /**
     * @brief toPackageOld This method convert this class object to the package.
     *  For more info see Package class.
     * @param package  This is return value of Package class.
     * @param triggerHash This is hash of the package the current class is responding to.
     * @return True if convert to package finished successful.
     */
    bool toPackageOld(Package &package, unsigned int triggerHash = 0) const;

    /**
     * @brief cmdOld - This is command of this object, (for generate cmd use macross QH_PACKAGE)
     * @note Use the QH_PACKAGE macross for implement this method.
     * @return global command of package.
     * @see QH_PACKAGE
     */
    virtual unsigned short cmdOld() const = 0;

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

    /**
     * @brief localCode This method return local code
     * @return local command of this class. used for check QH_PACKAGE macro before send pacakge.
     */
    virtual unsigned int localCode() const = 0;

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
