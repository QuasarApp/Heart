/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <abstractnode.h>
#include <heart.h>


class TestUtils
{
public:
    TestUtils();
    virtual ~TestUtils();
    bool wait(const std::function<bool()> &forWait, int msec) const;

    bool connectFunc(QH::AbstractNode *cli,
                     const QString &address,
                     unsigned short port) const;

    bool funcPrivateConnect(const std::function<bool ()> &requestFunc,
                            const std::function<bool ()> &checkFunc,
                            const std::function<QMetaObject::Connection ()> &connectFunction) const;

    bool funcPrivateConnect(const std::function<bool ()> &requestFunc,
                            const std::function<bool ()> &checkFunc) const;

};

#endif // TESTUTILS_H
