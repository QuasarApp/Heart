/*
 * Copyright (C) 2018-2021 QuasarApp.
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

inline void initResources() { Q_INIT_RESOURCE(ProtockolResusces); }

/**
 * @brief The QH namespace - QuasarApp Heart namespace. This namespace contains all classes of the Heart library.
 * Usage:
 * 1. First one you need to create a package for transporting data betwin server and client. For this you need to do Inheritance with the
 * QF::PKG::AbstrcatData class. So, You need to override 2 serialization methods of AbstrcatData and the copyFrom method.
 * Example: The package for transporting text data.
 * \code{cpp}
class MyPackage: public QH::AbstractData
{
public:
    MyPackage();

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
 * @note The method copyFrom is not necessary method so you can be skip it. Bud if you want override it then you need to override like in example, with check of object type. If you do not override this method or override it not correctly then copy data from another package do not work correctly. In base case method copy From not using, but it is necessary for DBObject class
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
 *
 * The basic principle of the library.
 *
 * Node - it is server or client implementation of any of AbstractNode class of it child classes.
 *  - The node receive raw data from another network connection.
 *  - After parsing a raw data the node convert a bytes array to QH::Package.
 *  - The Package create a new thread for working with received request, so, all working of package working in own threads.
 *  - Next, the Node invoke a QH::AbstractNode::parsePackage method. This method must be return QH::ParserResult.
 *    @note Do not forget invoke the super class parsePackage method.
 *  - The Last step it is invoke your override parsePackage method on your server or client class.
 *     IF you need to send responce then use a  bool sendData(PKG::AbstractData *resp,  const HostAddress& addere, const Header *req = nullptr).
 *
 * Work scheme:
 *\image html Async.svg width=800px
 */
namespace QH {
    /**
         * @brief init This method initialize default resources of the Heart Library.
         * @warning Do not Forget invoke this method before using library.
         *
         *  Example :
         *  @code {cpp}
         *  #include <heart.h>
         *  int main() {
         *      if (!QH::init()) {
         *          return 1;
         *      }
         *      // some code
         *      return 0
         *  }
         *  @endcode
         * @return true if all resources initialize successful.
         */
    bool init();

    /** @brief The PKG namesapce - this namespace contains all default packages of the Heart library.
     *  If you want create a pool request for Heart Library with own implemented packages
     *  you need to create a new package into a PKG namesapce.
     */
    namespace PKG {

    }
}

#endif // NETWORKPROTOCOL_H
