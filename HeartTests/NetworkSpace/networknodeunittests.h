/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef NETWORK_DBTESTS_H
#define NETWORK_DBTESTS_H

#include <networknode.h>
#include <templatedatabasenodeunittests.h>

/**
 * @brief The DbTestsNode class - this implementation of node gor testing database
 */
class NetworkNodeUnitTests: public TemplateDataBaseNodeUnitTests<QH::NetworkNode, QH::PKG::NodeObject>
{
public:
    NetworkNodeUnitTests();

protected:
    QH::PKG::NodeObject *randomMember() const override;
};

#endif // NETWORK_DBTESTS_H
