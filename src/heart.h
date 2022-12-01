/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKPROTOCOL_H
#define NETWORKPROTOCOL_H

#include "package.h"
#include "abstractnode.h"

inline void initResources() { Q_INIT_RESOURCE(ProtockolResusces); }

/**
 * @brief The QH namespace - QuasarApp Heart namespace. This namespace contains all classes of the Heart library.
 *
 * Usage:
 *
 * 1. First one, you need to create a package for transporting data between server and client. For this you need to do Inheritance with the
 * QF::PKG::AbstrcatData class.
 *
 * Example: The package for transporting text data.
 *
 * \code{cpp}
class MyPackage: public QH::AbstractData
{
public:
    MyPackage();

    // override this method for validation your package class
    bool isValid() const {
        return AbstractData::isValid();
    };                     /

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
 * \endcode
 * 2. You need to create a ApiParserClass - The API parser class should be inherited of the iParser class and implement all business logics of your client - server app.
 *
 * Example:
 * \code{cpp}
 *
 * class MyParser: public QH::iParser {
    public:
        MyParser(QH::AbstractNode* parentNode): QH::iParser(parentNode) {
            registerPackageType<MyPackage>();
            data = new BigPackage();
    }

    // iParser interface
    public:

    // override this method for processed received data.
    ParserResult parsePackage(const Package &pkg,
                              const AbstractNodeInfo *sender) {

        auto parentResult = AbstractNode::parsePackage(pkg, sender);
        if (parentResult != ParserResult::NotProcessed) {
            return parentResult;
        }

        auto result = commandHandler<MyPackage>(this, &MyClass::processMyPackage, pkg, sender, pkgHeader);
        if (result != QH::ParserResult::NotProcessed) {
            return result;
        }

        return ParserResult::NotProcessed;
    }

    bool processMyPackage(const QSharedPointer<MyPackage> &cardrequest,
                          const QH::AbstractNodeInfo *sender, const QH::Header &hdr) {

        BaseId requesterId = getSender(sender, &cardrequest);

        if (!cardrequest.isValid()) {
            badRequest(sender->networkAddress(), hdr);
            return ParserResult::Error;
        }

        cardrequest._data = "responce for client "

        // responce only for servers.
        if (nodeType() == QH::AbstractNode::NodeType::Server)
            sendData(cardrequest, sender->networkAddress(), &pkg.hdr);

        return ParserResult::Processed;
    }

    // This vesion of the parser (any digital value.) .
    int version() const override {return 0;};
    QString parserId() const override {return "MyParser";};

    };
    \endcode
 *
 *
 * 3. You need to create a Server class. For this, you need to do Inheritance with the QF::AbstrcatData class

*  Example:

* \code{cpp}
class TestingServer: public QH::AbstractNode {
Q_OBJECT
public:
    TestingServer() {
        addApiParser<MyParser>();
    }

    QH::AbstractNode::NodeType nodeType() const override {
        return QH::AbstractNode::NodeType::Server;
    }
};
 * \endcode
 *
 *  This is a simple echo server for our client - server application.
 *  For Run this serve use method QF::AbstrcatData::run
 *
 *  \code{cpp}
        int main () {
            TestingServer testServer();
            testServer.run("127.0.0.1", 7777)
        }
    \endcode

 * 4. Create a client application class.
 * Client and server must be inheritance from QF::AbstrcatData class to support parsing packages.
 * \code{cpp}
class TestingClient: public QH::AbstractNode {

public:
    TestingClient() {
        addApiParser<MyParser>();
    }

    QH::AbstractNode::NodeType nodeType() const override {
        return QH::AbstractNode::NodeType::Client;
    }
};
 * \endcode
 *
 * The basic principle of the library.
 *
 * Node - it is the server or client implementation of any AbstractNode child's classes.
 *  - The node receives raw data from another network connection.
 *  - After parsing a raw data, the node converts a bytes array to QH::Package.
 *  - The Package creates a new thread for working with received request, so, all working of package working in own threads.
 *  - Next, the Node invokes a QH::iParser::parsePackage method of selected parsers. This method must be return QH::ParserResult.
 *  - IF you need to send response then use a  unsigned int sendData(PKG::AbstractData *resp,  const HostAddress& addere, const Header *req = nullptr).
 *
 * **About parsers**
 *
 * The Parser it is an object that works with your packages. You can create multiple version's parser for compatibility between node versions. Node will select most actually version that known both nodes. For example, server known parsers with version 1, 2 and 3 but client can work only with 1 and 2, so nodes choose parser v2 because all nodes known about this parser version.
 *
 * You can create multiple types of the parsers. For example, you want to create an absolute new API and add it to the node. You need override
 * Example:
 *
 * \code{cpp}
 *  TestingClient() {
        iParser::addApiParser<MyParser_v1>();
        iParser::addApiParser<MyParser_v2>();
        iParser::addApiParser<MyParser_v3>();
        iParser::addApiParser<MyParser_v4>();

    }
 * \endcode
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
    bool HEARTSHARED_EXPORT init();

    /**
     * @brief heartLibVersion This method return version of library in format : major.commit.hash
     * @return
     */
    HEARTSHARED_EXPORT QString heartLibVersion();

    /** @brief The PKG namesapce - this namespace contains all default packages of the Heart library.
     *  If you want create a pool request for Heart Library with own implemented packages
     *  you need to create a new package into a PKG namesapce.
     */
    namespace PKG {

    }
}

#endif // NETWORKPROTOCOL_H
