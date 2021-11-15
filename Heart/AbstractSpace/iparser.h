#ifndef IPARSER_H
#define IPARSER_H

#include <QSharedPointer>
#include <abstractdata.h>

namespace QH {

class AbstractNodeInfo;

namespace PKG {
class AbstractData;
}


/**
 * @brief The ParserResult enum.
 * Error - parser detect a errorob package.
 * NotProcessed - the parser does not know what to do with the package or has not finished processing it.
 * Processed - the parser finished processing correctly.
 */
enum class ParserResult {
    /// parser detect a errorob package.
    Error = 0,
    /// the parser does not know what to do with the package or has not finished processing it.
    NotProcessed = 1,
    /// the parser finished processing correctly.
    Processed = 2
};

/**
 * @brief The iParser class This class provide functions for parsing income packages.
 * For yousing just override the iParser::parsePackage method.
 * @see AbstractNode::parsePackage
 * @see AbstractNode
 */
class iParser
{
public:
    iParser();

    /**
     * @brief parsePackage This is main method of all childs classes of an AbstractNode class.
     *  This method work on own thread.
     *  If you ovveride this method you need to create this than an example:
     * \code{cpp}
        ParserResult DataBaseNode::parsePackage(PKG::AbstractData *pkg,
                                                const Header& pkgHeader,
                                                const AbstractNodeInfo* sender) {
            auto parentResult = AbstractNode::parsePackage(pkg, sender);
            if (parentResult != ParserResult::NotProcessed) {
                return parentResult;
            }

            // you can use parsing without the commandHandler method
            if (MyCommand::command() == pkg->cmd()) {

                BaseId requesterId = getSender(sender, &obj);

                ...

                if (FailCondition) {
                    return ParserResult::Error;
                }

                ...

                return ParserResult::Processed;

            }

            // Or with the commandHandler method

            auto result = commandHandler<MyPackage>(this, &MyClass::processMyPackage, pkg, sender, pkgHeader);
            if (result != QH::ParserResult::NotProcessed) {
                return result;
            }

            return ParserResult::NotProcessed;
        }
     * \endcode
     * @param pkg This is package with incomming data.
     * @param sender This is sender this pacakge.
     * @param pkgHeader This is header of the incoming packet is used to create a response.
     * @return item of ParserResult. For more information see The ParserResult enum.
     * @see AbstractNode::commandHandler
     * @see AbstractNode::sendData
     * @see AbstractNode::badRequest

     */
    virtual ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                      const Header& pkgHeader, const AbstractNodeInfo* sender) = 0;

    /**
     * @brief pareseResultToString This method convert ParserResult value to string.
     * @return The String value of pareseresult.
     */
    QString pareseResultToString(const ParserResult& parseResult) const;

    /**
     * @brief version This method return version of parser object
     * @return version of parsed object.
     */
    virtual int version() const = 0;

    /**
     * @brief commandHandler This method it is simple wrapper for the handle pacakges in the AbstractNode::parsePackage method.
     * Exmaple of use :
     * @code{cpp}
     *      auto result = commandHandler<MyPackage>(this, &MyClass::processMyPackage, pkg, sender, pkgHeader);
            if (result != QH::ParserResult::NotProcessed) {
                return result;
            }
            ...
     * @endcode
     * @tparam PackageClass This is class name that you want handle.  All classes mist be inhert of the QH::PKG::AbstractData class.
     * @tparam HandlerType This is type of the handler object that will invoke @a HandlerMethod method.

     * @tparam HandlerMethod This is name of the handler method.
     * The handler method should be support next signature:
     *  **bool Method(const QSharedPointer<QH::PKG::PackageClass> &, const QH::Header &pkgHeader, const QH::AbstractNodeInfo *sender)**.
     * @param handlerObject This is pointer to handler object.
     * @param method This is handler method.
     * @param pkg This is package data from the AbstractNode::parsePackage argumetns
     * @param pkgHeader This is header of an incomming package.
     * @param sender This is socket object of a sender that send this package.
     * @return item of ParserResult. For more information see The ParserResult enum.
     *
     * @see iParser::parsePackage
     * @see ParserResult
     */
    template<class PackageClass,class HandlerType, class HandlerMethod>

    inline ParserResult commandHandler(HandlerType handlerObject, HandlerMethod method,
                                       const QSharedPointer<QH::PKG::AbstractData> &pkg,
                                       const QH::AbstractNodeInfo *sender,
                                       const QH::Header &pkgHeader) {

        if (PackageClass::command() == pkg->cmd()) {
            auto data = pkg.staticCast<PackageClass>();

            if (!data->isValid()) {
                return QH::ParserResult::Error;
            }

            if(!(handlerObject->*method)(data, sender, pkgHeader)) {
                return QH::ParserResult::Error;
            }

            return QH::ParserResult::Processed;
        }

        return QH::ParserResult::NotProcessed;
    }

};


}
#endif // IPARSER_H
