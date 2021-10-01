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

#define TIMEOUT_INTERVAL 30000

namespace QH {

BigDataManager::BigDataManager(AbstractNode *node)
{
    debug_assert(node, "BigDataManager requer node object");
    _node = node;
}

bool BigDataManager::newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                                const AbstractNodeInfo *sender,
                                const Header &pkgHeader) {
    header->getPackagesCount();

    if (!_pool.contains(header->packageId())) {
        QVector<QSharedPointer<PKG::BigDataPart>> _array;
        _array.reserve(header->getPackagesCount());
        _pool[header->packageId()] = {_array, static_cast<int>(time(0))};
    }

    checkOutDatedPacakges();

    return true;
}

bool BigDataManager::processPart(const QSharedPointer<PKG::BigDataPart> &part,
                                 const AbstractNodeInfo *sender,
                                 const Header &pkgHeader) {

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

    const auto& chain = _pool[footer->packageId()].chaindata;

    QByteArray packageRawData;
    QList<int> invalidParts;
    for (int idx = 0; idx < chain.size(); ++idx) {
        if (chain[idx]) {
            packageRawData += chain[idx]->data();
        } else {
            invalidParts.push_back(idx);
        }
    }

    if (invalidParts.isEmpty()) {
        Package pkg;
        if (!pkg.fromBytes(packageRawData)) {
            return false;
        }

        auto package = _node->prepareData(pkg);
        if (_node->parsePackage(package, pkgHeader, sender) == ParserResult::Error) {
            return false;
        }

    } else {

        PKG::BigDataRequest request;
        request.setNeededParts(invalidParts);
        request.setPackageId(footer->packageId());

        return _node->sendData(&request, sender, &pkgHeader);
    }

    return true;
}

void BigDataManager::checkOutDatedPacakges() {

    for (auto key = _pool.keyBegin(); key != _pool.keyEnd(); ++key) {

        if (time(0) - _pool[*key].lastUpdate > TIMEOUT_INTERVAL) {
            _pool.remove(*key);
        }
    }
}
}
