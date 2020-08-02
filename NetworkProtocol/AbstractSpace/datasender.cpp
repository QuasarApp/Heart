#include "datasender.h"
#include <QAbstractSocket>
#include <quasarapp.h>
#include <QThread>

namespace NP {

DataSender::DataSender() {
    QThread *own = new QThread(this);
    moveToThread(own);
}

void NP::DataSender::sendPackagePrivate(QByteArray array, QAbstractSocket *target) const {
    if (array.size() != target->write(array)) {
        QuasarAppUtils::Params::log("not writed data to socket", QuasarAppUtils::Error);
    }
}

}
