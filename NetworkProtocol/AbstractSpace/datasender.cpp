#include "datasender.h"
#include <QAbstractSocket>
#include <quasarapp.h>
#include <QThread>

namespace NP {

DataSender::DataSender() {
    QThread *own = new QThread(this);
    moveToThread(own);
    own->start();
}

void NP::DataSender::sendPackagePrivate(QByteArray array, void *target) const {
    auto ptr = static_cast<QAbstractSocket*>(target);
    if (array.size() != ptr->write(array)) {
        QuasarAppUtils::Params::log("not writed data to socket", QuasarAppUtils::Error);
    }

    if (!ptr->waitForBytesWritten()) {
        QuasarAppUtils::Params::log("not writed data to socket", QuasarAppUtils::Error);
    }

}

}
