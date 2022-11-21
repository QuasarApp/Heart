//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef APIVERSIONPARSER_H
#define APIVERSIONPARSER_H

#include "iparser.h"

namespace QH {


/**
 * @brief The APIVersionParser class This is main parser forthe main command.
 * This parsers work only with the APIVersion packge;
 */
class APIVersionParser: public iParser
{
public:
    APIVersionParser();

    // iParser interface
public:
    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              const AbstractNodeInfo *sender) override;
    int version() const override;
    QString parserId() const override;
};
}
#endif // APIVERSIONPARSER_H
