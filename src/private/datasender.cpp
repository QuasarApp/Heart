/*
 * Copyright (C) 2018-2025 QuasarApp.
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

    if (!(ptr && ptr->isValid() && ptr->isWritable())) {
        qCritical() << "Send raw data error. Socket is invalid";
        return false;
    }

    int wrote = 0;

    while (wrote < array.length() && ptr->isWritable()) {
        wrote += ptr->write(array.mid(wrote, array.length()));
    }

    if (!ptr->flush()) {
        qCritical() << "Send raw data error. data not flushed";
        return false;
    }

    if (array.size() != wrote) {
        qCritical() << "Send raw data error. not all data writed";
        return false;
    }

    return true;
}

}
