/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#include "datasender.h"
#include <QAbstractSocket>
#include <quasarapp.h>
#include <QThread>

namespace QH {

DataSender::DataSender(QThread *thread):
    Async(thread ) {

}

bool DataSender::sendData(const QByteArray &array, void *target, bool await) const {
    return asyncLauncher(std::bind(&DataSender::sendPackagePrivate, this, array, target), await);
}

bool QH::DataSender::sendPackagePrivate(QByteArray array, void *target) const {
    auto ptr = static_cast<QAbstractSocket*>(target);
    if (array.size() != ptr->write(array)) {
        QuasarAppUtils::Params::log("not writed data to socket", QuasarAppUtils::Error);
        return false;
    }

    return ptr->waitForBytesWritten();;
}

}
