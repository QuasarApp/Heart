#ifndef IWORKER_H
#define IWORKER_H

#include "abstractnode.h"

namespace QH {


/**
 * @brief The IWorker class is base interface for create node workers.
 */
class IParser
{
public:
    IParser(AbstractNode* node);

    /**
     * @brief prepareData This is private method for preparing package from the byteArray.
     * @param pkg This is a raw package value.
     * @return pointer into prepared data.
     * @warning The return value do not clear automatically.
     * @see iWorker::genPackage
     * @see iWorker::checkCommand
     * @see iWorker::registerPackageType
     * @see Header::command
     */
    QSharedPointer<PKG::AbstractData> prepareData(const Package& pkg) const;

    /**
     * @brief genPackage This method will generate pacakge object from command. This return value will be invalid, for create a full package object use the iWorker::prepareData method
     * @param cmd This is integer command of pacakge.
     * @return empty pacakge object of the @a cmd.
     * @see iWorker::prepareData
     * @see iWorker::checkCommand
     * @see iWorker::registerPackageType
     * @see Header::command
     */
    QSharedPointer<PKG::AbstractData> genPackage(unsigned short cmd) const;

    /**
     * @brief checkCommand This method check command are if registered type or not.
     * @brief cmd This is command of a verifiable package.
     * @return True if the package is registered in a node.
     * @see iWorker::prepareData
     * @see iWorker::genPackage
     * @see iWorker::registerPackageType
     * @see Header::command
     */
    bool checkCommand(unsigned short cmd) const;

    /**
     * @brief version This method should be return current version of the package parser.
     *  The base implementation return 0.
     * @return version number.
     * @see Header::version
     */
    virtual unsigned short version() const;

    template<class T>
    /**
     * @brief registerPackageType This method register package type T.
     * This is need to prepare pacakge for parsing in the parsePackage method of the current worker modeule.
     *
     *
     * **Example of use**:
     *
     * @code{cpp}
     *
     * // this is constructor of the node:
     *  // create new parser object
     *
     *  auto parser = QSharedPointer<QH::AbstractNodeParser>::create(this);

        // register new package for parser. Note this package can't parse but the node emit incomePacakge siganl with received data.
        parser->registerPackageType<BigPackage>();
        // register your parser for node.
        addParser(parser);
     * @endcode
     */
    void registerPackageType() {
        _registeredTypes[T::command()] = [](){
            return new T();
        };
    };

    /**
     * @brief parsePackage This is main method of all childs classes of an AbstractNode and IWorker classes.
     *  This method work on own thread.
     *  If you ovveride this method you need to create this than an example:
     * \code{cpp}
        ParserResult IWorker::parsePackage(PKG::AbstractData *pkg,
                                                const Header& pkgHeader,
                                                const AbstractNodeInfo* sender) {
            // where the WorkerClass it is parent worker class .
            auto parentResult = WorkerClass::parsePackage(pkg, sender);
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
     * @see IWorker::commandHandler
     * @see AbstractNode::sendData
     * @see AbstractNode::badRequest

     */
    virtual ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                      const Header& pkgHeader, const AbstractNodeInfo* sender) = 0;



protected:
    /**
     * @brief node This method return pointer to current node object.
     * @tparam NodeClass This is class name of the returned type.
     * @return current node object.
     */
    template<class NodeClass>
    NodeClass* node() const {

        static_assert (std::is_base_of<NodeClass, AbstractNode>::value,
                "The NodeClass must be inherit of AbstractNode class.");

        return static_cast<NodeClass*>(_node);
    }

    /**
     * @brief commandHandler This method it is simple wrapper for the handle pacakges in the IWorker::parsePackage method.
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
     * @see IWorker::parsePackage
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

private:
    AbstractNode* _node = nullptr;
    QHash<unsigned short, std::function<PKG::AbstractData*()>> _registeredTypes;

};

}
#endif // IWORKER_H
