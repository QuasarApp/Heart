#include "iparser.h"
#include "quasarapp.h"

namespace QH {

IParser::IParser(AbstractNode* node) {
    _node = node;
}

QSharedPointer<PKG::AbstractData> IParser::prepareData(const Package &pkg) const {

    auto value = genPackage (pkg.hdr.command);
    if (!value) {
        QuasarAppUtils::Params::log("You try parse not registered package type."
                                    " Plese use the registerPackageType method befor parsing."
                                    " Example invoke registerPackageType<MyData>() into constructor of you client and server nodes.");

        return nullptr;
    }

    value->fromPakcage(pkg);
    return value;
}

QSharedPointer<PKG::AbstractData> IParser::genPackage(unsigned short cmd) const {
    return QSharedPointer<PKG::AbstractData>(_registeredTypes.value(cmd, [](){return nullptr;})());
}

bool IParser::checkCommand(unsigned short cmd) const {
    return _registeredTypes.contains(cmd);
}

unsigned short IParser::version() const {
    return 0;
}

}
