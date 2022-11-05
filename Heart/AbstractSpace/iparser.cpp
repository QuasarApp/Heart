#include "iparser.h"

#include <abstractdata.h>

namespace QH {

iParser::iParser() {

}


QString iParser::pareseResultToString(const ParserResult &parseResult) const {
    switch (parseResult)
    {
    case ParserResult::Processed:
        return "Processed";
    case ParserResult::NotProcessed:
        return "NotProcessed";

    default: return "Error";
    }
}

const QHash<unsigned short, std::function<PKG::AbstractData *()> > &
iParser::registeredTypes() const {
    return _registeredTypes;
}

QSharedPointer<PKG::AbstractData> iParser::genPackage(unsigned short cmd) const {
    return QSharedPointer<PKG::AbstractData>(_registeredTypes.value(cmd, [](){return nullptr;})());
}

bool iParser::checkCommand(unsigned short cmd) const {
    return _registeredTypes.contains(cmd);
}
}
