#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <abstractnode.h>
#include <networkprotocol.h>


class TestUtils
{
public:
    TestUtils();
    bool wait(const std::function<bool()> &forWait, int msec);

    bool connectFunc(NP::AbstractNode *cli,
                     const QString &address,
                     unsigned short port);

    bool funcPrivateConnect(const std::function<bool ()> &requestFunc,
                            const std::function<bool ()> &checkFunc,
                            const std::function<QMetaObject::Connection ()> &connectFunction);

    bool funcPrivateConnect(const std::function<bool ()> &requestFunc,
                            const std::function<bool ()> &checkFunc);

};

#endif // TESTUTILS_H
