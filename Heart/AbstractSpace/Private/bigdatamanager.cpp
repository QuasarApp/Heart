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

void QH::BigDataManager::insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header) {
    if (!_pool.contains(header->packageId())) {
        QVector<QSharedPointer<PKG::BigDataPart>> _array;
        _array.resize(header->getPackagesCount());
        _pool[header->packageId()] = {header, _array, static_cast<int>(time(0))};
    }

    checkOutDatedPacakges(header->packageId());
}

bool BigDataManager::newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                                const AbstractNodeInfo *sender,
                                const Header & hdr) {

    if (!header->isValid())
        return false;

    insertNewBigData(header);

    PKG::BigDataRequest request;
    request.setCurrentPart(0);
    request.setPackageId(header->packageId());

    return _node->sendData(&request, sender, &hdr);
}

bool BigDataManager::processPart(const QSharedPointer<PKG::BigDataPart> &part,
                                 const AbstractNodeInfo *sender,
                                 const Header & hdr) {

    if (!_pool.contains(part->packageId())) {
        return false;
    }

    checkOutDatedPacakges(part->packageId());

    auto& localPool = _pool[part->packageId()];

    localPool.chaindata[part->getPakckageNumber()] = part;

    auto sendRequest = [sender, &hdr, this](int part, unsigned int id){

        PKG::BigDataRequest request;
        request.setCurrentPart(part);
        request.setPackageId(id);

        return _node->sendData(&request, sender, &hdr);
    };

    if (part->getPakckageNumber() + 1 < localPool.chaindata.size()) {
        return sendRequest(part->getPakckageNumber() + 1,
                           part->packageId());

    }

    auto package = _node->genPackage(localPool.header->getCommand());
    if (!package)
        return false;

    QByteArray packageRawData;
    for (int idx = 0; idx < localPool.chaindata.size(); ++idx) {
        if (localPool.chaindata[idx]) {
            packageRawData += localPool.chaindata[idx]->data();
        } else {
            return sendRequest(idx, part->packageId());
        }
    }

    package->fromBytes(packageRawData);

    if (_node->parsePackage(package, hdr, sender) == ParserResult::Error) {
        return false;
    }

    _pool.remove(part->packageId());

    return true;
}

bool BigDataManager::processRequest(const QSharedPointer<PKG::BigDataRequest> &request,
                                    const AbstractNodeInfo *sender,
                                    const Header &pkgHeader) {


    unsigned int id = request->packageId();

    if (!_pool.contains(id)) {
        QuasarAppUtils::Params::log("requested data is missing!");
        return false;
    }

    checkOutDatedPacakges(id);

    const auto &localPool = _pool[id];
    if (request->currentPart() >= localPool.chaindata.size()) {
        return false;
    }

    bool fLast = localPool.chaindata.size() - 1 == request->currentPart();

    const auto &data = localPool.chaindata[request->currentPart()];
    if (!_node->sendData(data.data(), sender, &pkgHeader)) {
        return false;
    }

    if (fLast) {
        _pool.remove(id);
    }

    return true;
}

bool BigDataManager::sendBigDataPackage(const PKG::AbstractData *data,
                                        const AbstractNodeInfo *sender,
                                        const QH::Header *pkgHeader) {

    unsigned int sizeLimit = 32000;
    auto rawData = data->toBytes();

    auto hdr = QSharedPointer<PKG::BigDataHeader>::create();
    hdr->setPackagesCount(std::ceil(rawData.size() / static_cast<double>(sizeLimit)));
    hdr->setPackageId(rand());
    hdr->setCommand(data->cmd());

    insertNewBigData(hdr);

    for (int i = 0; i < hdr->getPackagesCount(); ++i) {
        auto part = QSharedPointer<PKG::BigDataPart>::create();
        part->setPackageId(hdr->packageId());
        part->setPakckageNumber(i);
        part->setData(rawData.mid(i * sizeLimit, sizeLimit));

        _pool[hdr->packageId()].chaindata[i] = part;
    }


    if (!_node->sendData(hdr.data(), sender, pkgHeader)) {
        return false;
    }

    return true;
}

void BigDataManager::checkOutDatedPacakges(unsigned int currentProcessedId) {
    int utx = time(0);

    if (_pool.contains(currentProcessedId)) {
        _pool[currentProcessedId].lastUpdate = utx;
    }

    for (auto key = _pool.keyBegin(); key != _pool.keyEnd(); ++key) {
        if (utx - _pool[*key].lastUpdate > TIMEOUT_INTERVAL) {
            _pool.remove(*key);
        }
    }
}

}