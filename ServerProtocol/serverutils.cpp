#include "serverutils.h"
#include <QtDebug>
#include <quasarapp.h>
#include <unistd.h>

ServerUtils::ServerUtils() {

}

void ServerUtils::helpDaemon() {

    QStringList help =
    {{ ""},
    { "Usage: SnakeServer-daemon.sh <[params]> "},
    { ""},
    { "Options:"},
    { "   help / h                 : show help."},
    { "   daemon / d               : start like daemon."},
    { "   -port (port)             : start with custom port."},
    { "   -address (address)       : start with custom address."},
    { "   -db (path/to/db.file)    : start with custom db"},
    { ""},
    { "General Options:"}};

    help.append(QuasarAppUtils::Params::getparamsHelp());
    QuasarAppUtils::Params::showHelp(help);

}

void ServerUtils::helpClient() {

    QStringList help =
    {{ ""},
    { "Usage: Terminal.sh <[params]> "},
    { ""},
    { "Options:"},
    { "   help / h                 : show help."},
    { "   ping                     : debug commnad"},
    { "   state                    : show information about deamon"},
    { "   stop                     : stop server deamon"},
    { "   -ban (address)           : ban user with address"},
    { "   -unban (address)         : unban user with address"},
    { "   -restart (address:port)  : restarrt server deamon with new address and port"},
    { "   -start (address:port)    : start server deamon with custom address"},
    { "   start                    : start server deamon with default address"},
    { ""},
    { "General Options:"},};

    help.append(QuasarAppUtils::Params::getparamsHelp());
    QuasarAppUtils::Params::showHelp(help);
}

bool ServerUtils::runDaemon() {

#ifdef Q_OS_UNIX
    if (QuasarAppUtils::Params::isEndable("daemon") ||
            QuasarAppUtils::Params::isEndable("d")) {

        int pid = fork();
        if (pid != 0) {
            return true;
        }
    }

#endif
    return false;
}

bool ServerUtils::parseParams(int argc, char *argv[]) {
    if ( !QuasarAppUtils::Params::parseParams(argc, const_cast<const char**>(argv))) {
        return false;
    }

    return true;
}
