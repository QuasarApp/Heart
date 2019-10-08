#include <QCoreApplication>
#include <quasarapp.h>
#include <client.h>
#include <QDebug>
#include <serverutils.h>
#include <QTimer>

void handleResponcke(const QVariantMap &data) {
    for(auto iter = data.begin(); iter != data.end(); ++iter) {
        qInfo() << QString("%0: %1").arg( iter.key()).arg(iter.value().toString());
    }

    QCoreApplication::exit(0);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QuasarAppUtils::Params::parseParams(argc, const_cast<const char**>(argv));

    ServerProtocol::Client cli;

    QObject::connect(&cli, &ServerProtocol::Client::sigIncommingData,
                     &handleResponcke);

    if (QuasarAppUtils::Params::isEndable("ping")) {

        if (!cli.ping()) {
            qCritical() << "command not sendet!";
            return 1;
        }

    } else if (QuasarAppUtils::Params::isEndable("help") ||
               QuasarAppUtils::Params::isEndable("h")) {

        ServerUtils::helpClient();
        return 0;

    } else if (QuasarAppUtils::Params::isEndable("state")) {

        if (!cli.getState()) {
            qCritical() << "command not sendet!";
            return 1;
        };

    } else if (QuasarAppUtils::Params::isEndable("ban")) {

        auto address = QuasarAppUtils::Params::getStrArg("ban");

        if (!cli.ban(QHostAddress(address))) {
            qCritical() << "command not sendet!";
            return 1;
        }

    } else if (QuasarAppUtils::Params::isEndable("unban")) {

        auto address = QuasarAppUtils::Params::getStrArg("unban");
        if (!cli.unBan(QHostAddress(address))) {
            qCritical() << "command not sendet!";
            return 1;
        }

    } else if (QuasarAppUtils::Params::isEndable("restart")) {

        QStringList address = QuasarAppUtils::Params::getStrArg("restart").split(":");
        if (!cli.restart(address.value(0), address.value(1).toUShort())) {
            qCritical() << "command not sendet!";
            return 1;
        }
    } else if (QuasarAppUtils::Params::isEndable("stop")) {

        if (!cli.stop()) {
            qCritical() << "command not sendet!";
            return 1;
        }
    } else if (QuasarAppUtils::Params::isEndable("start")) {

        QStringList params = QuasarAppUtils::Params::getStrArg("start").split(":");

        if (!cli.start(params.value(0), params.value(1).toUShort())) {
            qCritical() << "Server not started!";
            return 1;
        }

        return 0;
    }
    else {
        ServerUtils::helpClient();
        return 0;
    }

    QTimer::singleShot(3000, [&a] {
        qCritical() << "server not responsed !";
        a.exit(1);
    });

    return a.exec();
}
