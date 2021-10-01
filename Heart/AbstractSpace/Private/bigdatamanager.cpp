/*
 * Copyright (C) 2021-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdatamanager.h"
#include "quasarapp.h"
#include <QSharedPointer>
#include <QVector>
#include <abstractnode.h>
#include <bigdatarequest.h>
#include <cmath>

#define TIMEOUT_INTERVAL 30000

namespace QH {

BigDataManager::BigDataManager(AbstractNode *node)
{
    debug_assert(node, "BigDataManager requer node object");
    _node = node;
}

bool BigDataManager::newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                                const AbstractNodeInfo *,
                                const Header &) {

    if (!header->isValid())
        return false;

    if (!_pool.contains(header->packageId())) {
        QVector<QSharedPointer<PKG::BigDataPart>> _array;
        _array.reserve(header->getPackagesCount());
        _pool[header->packageId()] = {header, _array, static_cast<int>(time(0))};
    }

    checkOutDatedPacakges();

    return true;
}

bool BigDataManager::processPart(const QSharedPointer<PKG::BigDataPart> &part,
                                 const AbstractNodeInfo *,
                                 const Header &) {

    if (!_pool.contains(part->packageId())) {
        return false;
    }

    _pool[part->packageId()].chaindata[part->getPakckageNumber()] = part;
    _pool[part->packageId()].lastUpdate = time(0);

    checkOutDatedPacakges();
    return true;
}

bool BigDataManager::finishPart(const QSharedPointer<PKG::BigDataFooter> &footer,
                                const AbstractNodeInfo *sender,
                                const Header &pkgHeader) {

    _pool[footer->packageId()].lastUpdate = time(0);
    checkOutDatedPacakges();

    const auto& localPool = _pool[footer->packageId()];

    QByteArray packageRawData;
    QList<int> invalidParts;
    for (int idx = 0; idx < localPool.chaindata.size(); ++idx) {
        if (localPool.chaindata[idx]) {
            packageRawData += localPool.chaindata[idx]->data();
        } else {
            invalidParts.push_back(idx);
        }
    }

    if (invalidParts.isEmpty()) {

        auto package = _node->genPackage(localPool.header->getCommand());
        package->fromBytes(packageRawData);

        if (_node->parsePackage(package, pkgHeader, sender) == ParserResult::Error) {
            return false;
        }
    }

    PKG::BigDataRequest request;
    request.setNeededParts(invalidParts);
    request.setPackageId(footer->packageId());

    return _node->sendData(&request, sender, &pkgHeader);
}

bool BigDataManager::processRequest(const QSharedPointer<PKG::BigDataRequest> &request,
                                    const AbstractNodeInfo *sender,
                                    const Header &pkgHeader) {

    if (request->isFinishedSuccessful()) {
        _pool.remove(request->packageId());
        return true;
    }

    return sendParts(request->packageId(), sender, pkgHeader, request->neededParts());

}

bool BigDataManager::sendBigDataPackage(const QSharedPointer<PKG::AbstractData> &data,
                                        const AbstractNodeInfo *sender,
                                        const QH::Header &pkgHeader) {



    unsigned int sizeLimit = std::numeric_limits<decltype (pkgHeader.size)>::max();
    auto rawData = data->toBytes();

    auto hdr = QSharedPointer<PKG::BigDataHeader>::create();
    hdr->setPackagesCount(std::ceil(rawData.size() / static_cast<double>(sizeLimit)));
    hdr->setPackageId(rand());
    hdr->setCommand(data->cmd());

    if (!newPackage(hdr, {}, {})) {
        return false;
    }

    if (!_node->sendData(hdr.data(), sender, &pkgHeader)) {
        return false;
    }

    for (int i = 0; i < hdr->getPackagesCount(); ++i) {
        auto part = QSharedPointer<PKG::BigDataPart>::create();
        part->setPackageId(hdr->packageId());
        part->setPakckageNumber(i);
        part->setData(rawData.mid(i * sizeLimit, sizeLimit));

        _pool[hdr->packageId()].chaindata[i] = part;

        if (!_node->sendData(part.data(), sender, &pkgHeader)) {
            return false;
        }
    }

    auto footer = QSharedPointer<PKG::BigDataFooter>::create();
    footer->setPackageId(hdr->packageId());

    return _node->sendData(footer.data(), sender, &pkgHeader);
}

void BigDataManager::checkOutDatedPacakges() {

    for (auto key = _pool.keyBegin(); key != _pool.keyEnd(); ++key) {
        if (time(0) - _pool[*key].lastUpdate > TIMEOUT_INTERVAL) {
            _pool.remove(*key);
        }
    }
}

bool BigDataManager::sendParts(int packageId,
                               const AbstractNodeInfo *sender,
                               const Header &pkgHeader,
                               const QList<int> requiredParts) {

    auto localPool = _pool.value(packageId);
    localPool.lastUpdate = time(0);

    for (int id: requiredParts) {
        if (id >= localPool.chaindata.size()) {
            return false;
        }

        const auto &data = localPool.chaindata[id];
        _node->sendData(data.data(), sender, &pkgHeader);
    }

    PKG::BigDataFooter footer;
    footer.setPackageId(packageId);

    return _node->sendData(&footer, sender, &pkgHeader);

}
}
