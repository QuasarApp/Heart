/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKPROTOCOL_H
#define NETWORKPROTOCOL_H

#include "package.h"
#if HEART_BUILD_LVL >= 0
#include "abstractnode.h"
#endif

#if HEART_BUILD_LVL >= 1
#include "databasenode.h"
#endif

#if HEART_BUILD_LVL >= 2
#include "networknode.h"
#endif

/**
 * @brief The QH namespace - QuasarApp Heart namespace. This namespace contains all classes of the Heart library.
 * Usage:
 * 1. First one you need to create a package for transporting data betwin server and client. For this you need to do Inheritance with the
 * QF::PKG::AbstrcatData class. So You need to override 2 serialization methods of AbstrcatData and method copyFrom.
 * Example package for transporting text data.
 * \code{cpp}
class MyPackage: public QH::AbstractData
{
public:
    MyPackage();

    // constructor for create object from package
    MyPackage(const Package& from): QH::AbstractData(from) {

    };

    // override this method for validation your package class
    bool isValid() const {
        return AbstractData::isValid();
    };                     /
    // override this method for copy object data from other to this object
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
 * \endcode
 * @note The method copyFrom is not nicessery method so yo can be skip it.
 * 2. You need to create a Server class. For this you need to do Inheritance with the QF::AbstrcatData class
 *  Example:
 * \code{cpp}
class TestingServer: public QH::AbstractNode {

protected:
    // override this method for processed received data.
    ParserResult parsePackage(const Package &pkg,
                                            const AbstractNodeInfo *sender) {

        auto parentResult = AbstractNode::parsePackage(pkg, sender);
        if (parentResult != ParserResult::NotProcessed) {
            return parentResult;
        }

        if (H_16<MyPackage>() == pkg.hdr.command) {
            MyPackage obj(pkg);

            BaseId requesterId = getSender(sender, &obj);

            if (!obj.isValid()) {
                badRequest(sender->networkAddress(), pkg.hdr);
                return ParserResult::Error;
            }

            obj._data = "responce for client "

            SendData(&obj, sender->networkAddress(), &pkg.hdr);
            return ParserResult::Processed;
        }
        return ParserResult::NotProcessed;

    }
};
 * \endcode
 *  This is simple echo server for our client - server application.
 *  For Run this serve use method QF::AbstrcatData::run
 *
 *  \code{cpp}
        int main () {
            TestingServer testServer();
            testServer.run("127.0.0.1", 7777)
        }
    \endcode
 * 3. Create a client application class.
 * Client and server must be inheritance from QF::AbstrcatData class for support parsing packages.
 * \code{cpp}
class TestingClient: public QH::AbstractNode {


protected:
    // parsing incoming packages
    ParserResult parsePackage(const Package &pkg,
                                            const AbstractNodeInfo *sender) {

        auto parentResult = AbstractNode::parsePackage(pkg, sender);
        if (parentResult != ParserResult::NotProcessed) {
            return parentResult;
        }

        if (H_16<MyPackage>() == pkg.hdr.command) {
            MyPackage obj(pkg);

            // print responce of server
            std::cout << obj._data;
            ...
            return ParserResult::Processed;
        }
        // Do not forget return status of parsing packages
        return ParserResult::NotProcessed;

    }
    // sending request to server
    bool sendMyPackage() {
        Ping cmd;
        return sendData(&cmd, address);
    }
};
 * \endcode
 */
namespace QH {
    namespace PKG {

    }
}

#endif // NETWORKPROTOCOL_H
