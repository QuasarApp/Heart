/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef VERSIONPARSER_H
#define VERSIONPARSER_H

#include <iparser.h>

namespace QH {

/**
 * @brief The VersionParser class This is base parser. This parser have only one version is 0 because node du not has any version information in this step.
 */
class VersionParser: public QH::iParser
{
public:
    VersionParser();

    // iParser interface
public:
    ParserResult parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                              const Header &pkgHeader,
                              const AbstractNodeInfo *sender) override;
    int version() const override;

    QString parserId() const override;
};
}
#endif // VERSIONPARSER_H
