/*
 * Copyright (C) 2022-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "bigdataparser.h"
#include "bigdataheader.h"
#include "bigdatapart.h"

#include <bigdatarequest.h>
#include <abstractnode.h>
#include <cmath>
#include <params.h>
#include <bigdatawraper.h>

#define TIMEOUT_INTERVAL 30000

namespace QH {

BigDataParser::BigDataParser(AbstractNode* parentNode): iParser(parentNode) {

    registerPackageType<PKG::BigDataWraper>();
    registerPackageType<PKG::BigDataRequest>();
    registerPackageType<PKG::BigDataHeader>();
    registerPackageType<PKG::BigDataPart>();

}

ParserResult BigDataParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                         const Header &pkgHeader,
                                         AbstractNodeInfo *sender) {


    auto result = commandHandler<PKG::BigDataRequest>(this,
                                                      &BigDataParser::processRequest,
                                                      pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    result = commandHandler<PKG::BigDataWraper>(this,
                                                &BigDataParser::processBigDataWraper,
                                                pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    result = commandHandler<PKG::BigDataHeader>(this,
                                                &BigDataParser::newPackage,
                                                pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    result = commandHandler<PKG::BigDataPart>(this,
                                              &BigDataParser::processPart,
                                              pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    return ParserResult::NotProcessed;
}

int BigDataParser::version() const {
    return 1;
}

QString BigDataParser::parserId() const {
    return "HeartBigDataAPI";
}

void QH::BigDataParser::insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header) {
    if (!_pool.contains(header->packageId())) {
        QVector<QSharedPointer<PKG::BigDataPart>> _array;
        _array.resize(header->getPackagesCount());
        _pool[header->packageId()] = {header, _array, static_cast<int>(time(0))};
    }

    checkOutDatedPacakges(header->packageId());
}

bool BigDataParser::newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                                AbstractNodeInfo *sender,
                                const Header & hdr) {

    if (!header->isValid())
        return false;

    insertNewBigData(header);

    PKG::BigDataRequest request;
    request.setCurrentPart(0);
    request.setPackageId(header->packageId());

    QuasarAppUtils::Params::log("Receive BigData Header:" + header->toString());

    return node()->sendData(&request, sender, &hdr);
}

bool BigDataParser::processPart(const QSharedPointer<PKG::BigDataPart> &part,
                                 AbstractNodeInfo *sender,
                                 const Header & hdr) {

    if (!_pool.contains(part->packageId())) {
        return false;
    }

    checkOutDatedPacakges(part->packageId());

    auto& localPool = _pool[part->packageId()];

    QuasarAppUtils::Params::log(QString("Process Part of %0: part %1/%2").
                                arg(part->packageId()).
                                arg(part->getPakckageNumber()).
                                arg(localPool.chaindata.size() - 1));

    localPool.chaindata[part->getPakckageNumber()] = part;

    auto sendRequest = [sender, &hdr, this](int part, unsigned int id){

        PKG::BigDataRequest request;
        request.setCurrentPart(part);
        request.setPackageId(id);

        return node()->sendData(&request, sender, &hdr);
    };

    if (part->getPakckageNumber() + 1 < localPool.chaindata.size()) {
        return sendRequest(part->getPakckageNumber() + 1,
                           part->packageId());

    }

    auto package = node()->genPackage(localPool.header->getCommand(),
                                      sender);
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

    if (node()->parsePackage(package, hdr, sender) == ParserResult::Error) {
        return false;
    }

    _pool.remove(part->packageId());

    return true;
}

bool BigDataParser::processRequest(const QSharedPointer<PKG::BigDataRequest> &request,
                                    AbstractNodeInfo *sender,
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
    if (!node()->sendData(data.data(), sender, &pkgHeader)) {
        return false;
    }

    if (fLast) {
        _pool.remove(id);
    }

    return true;
}

bool BigDataParser::processBigDataWraper(const QSharedPointer<PKG::BigDataWraper> &request,
                                         AbstractNodeInfo *sender,
                                         const Header &pkgHeader) {
    return sendBigDataPackage(request->data(), sender, &pkgHeader);
}

bool BigDataParser::sendBigDataPackage(const PKG::AbstractData *data,
                                        const AbstractNodeInfo *sender,
                                        const QH::Header *pkgHeader) {

    unsigned int sizeLimit = Package::maximumSize() - sizeof (PKG::BigDataPart);
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


    if (!node()->sendData(hdr.data(), sender, pkgHeader)) {
        return false;
    }

    return true;
}

void BigDataParser::checkOutDatedPacakges(unsigned int currentProcessedId) {
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
