#ifndef SERVERUTILS_H
#define SERVERUTILS_H
#include "serverprotocol_global.h"


class SERVERPROTOCOLSHARED_EXPORT ServerUtils
{
public:

    ServerUtils();
    static void helpDaemon();
    static void helpClient();
    /**
     * @brief runDaemon
     * @return true if app running like a daemon
     */
    static bool runDaemon();
    static bool parseParams(int argc, char *argv[]);
};

#endif // SERVERUTILS_H
