#include <QCoreApplication>
#include <serverutils.h>
#include <quasarapp.h>
#include <serverutils.h>
#include <mainserver.h>

int main(int argc, char *argv[])
{
    if (!ServerUtils::parseParams(argc, argv)) {
        ServerUtils::helpDaemon();
        return 1;
    }

    if (QuasarAppUtils::Params::isEndable("help") ||
            QuasarAppUtils::Params::isEndable("h")) {
        ServerUtils::helpDaemon();
        return 0;
    }

    QString address = "";
    unsigned short port = 0;
    QString db = "";
    if(QuasarAppUtils::Params::isEndable("address")) {
        address = QuasarAppUtils::Params::getStrArg("address");
    }

    if(QuasarAppUtils::Params::isEndable("port")) {
        port = static_cast<unsigned short>(QuasarAppUtils::Params::getArg("port").toUInt());
    }

    if(QuasarAppUtils::Params::isEndable("db")) {
        db = QuasarAppUtils::Params::getStrArg("db");
    }


    if(ServerUtils::runDaemon()) {
        return 0;
    }

    QCoreApplication a(argc, argv);

    MainServer mainServer(false, nullptr);

    if (!mainServer.run(address, port, db)) {
        QuasarAppUtils::Params::verboseLog("server is not run!");
        ServerUtils::helpDaemon();
        return 1;
    }

    return a.exec();
}
