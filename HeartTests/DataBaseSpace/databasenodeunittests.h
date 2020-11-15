/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef DBTESTS_H
#define DBTESTS_H

#include <databasenode.h>
#include <usermember.h>
#include "templatedatabasenodeunittests.h"

/**
 * @brief The TemplateDataBaseNodeUnitTests class
 */
class DataBaseNodeUnitTests: public TemplateDataBaseNodeUnitTests<QH::DataBaseNode, QH::PKG::UserMember> {
public:
    DataBaseNodeUnitTests();

    // TemplateDataBaseNodeUnitTests interface
protected:
    const QH::PKG::UserMember *randomMember() const override;
};

#endif // DBTESTS_H
