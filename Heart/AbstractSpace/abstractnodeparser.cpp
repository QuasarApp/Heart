#include "abstractnodeparser.h"
#include <badrequest.h>
#include <bigdataheader.h>
#include <bigdatapart.h>
#include <bigdatarequest.h>
#include <closeconnection.h>
#include <ping.h>
#include <quasarapp.h>
#include <math.h>

namespace QH {

#define TIMEOUT_INTERVAL 30000
#define thiz node<AbstractNode>()

AbstractNodeParser::AbstractNodeParser(AbstractNode* node): IParser(node) {

    registerPackageType<PKG::Ping>();
    registerPackageType<PKG::BadRequest>();
    registerPackageType<PKG::CloseConnection>();

    registerPackageType<PKG::BigDataRequest>();
    registerPackageType<PKG::BigDataHeader>();
    registerPackageType<PKG::BigDataPart>();
}

ParserResult AbstractNodeParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                              const Header &pkgHeader,
                                              const AbstractNodeInfo *sender) {

    if (!thiz)
        return ParserResult::Error;

    if (!(sender)) {
        QuasarAppUtils::Params::log("sender socket is not valid!",
                                    QuasarAppUtils::Error);
        return ParserResult::Error;
    }

    if (!pkg->isValid()) {
        QuasarAppUtils::Params::log("incomming package is not valid!",
                                    QuasarAppUtils::Error);
        thiz->changeTrust(sender->networkAddress(), CRITICAL_ERROOR);
        return ParserResult::Error;
    }

    thiz->incomingData(pkg.data(), sender);

    if (PKG::Ping::command() == pkg->cmd()) {
        auto cmd = static_cast<PKG::Ping *>(pkg.data());
        if (!cmd->ansver()) {
            cmd->setAnsver(true);
            thiz->sendData(cmd, sender, &pkgHeader);
        }

        return ParserResult::Processed;
    } else if (PKG::BadRequest::command() == pkg->cmd()) {
        auto cmd = static_cast<PKG::BadRequest *>(pkg.data());

        emit thiz->requestError(cmd->errCode(), cmd->err());

        return ParserResult::Processed;

    } else if (PKG::CloseConnection::command() == pkg->cmd()) {

        if (sender->isLocal()) {
            thiz->removeNode(sender->networkAddress());
        }
        return ParserResult::Processed;
    }

    auto result = commandHandler<PKG::BigDataRequest>(this,
                                                 &AbstractNodeParser::processRequest,
                                                 pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    result = commandHandler<PKG::BigDataHeader>(this,
                                           &AbstractNodeParser::newPackage,
                                           pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    result = commandHandler<PKG::BigDataPart>(this,
                                         &AbstractNodeParser::processPart,
                                         pkg, sender, pkgHeader);
    if (result != QH::ParserResult::NotProcessed) {
        return result;
    }

    return ParserResult::NotProcessed;
}


void AbstractNodeParser::insertNewBigData(const QSharedPointer<PKG::BigDataHeader> &header) {
    if (!_pool.contains(header->packageId())) {
        QVector<QSharedPointer<PKG::BigDataPart>> _array;
        _array.resize(header->getPackagesCount());
        _pool[header->packageId()] = {header, _array, static_cast<int>(time(0))};
    }

    checkOutDatedPacakges(header->packageId());
}

bool AbstractNodeParser::newPackage(const QSharedPointer<PKG::BigDataHeader> &header,
                                const AbstractNodeInfo *sender,
                                const Header & hdr) {

    if (!header->isValid())
        return false;

    insertNewBigData(header);

    PKG::BigDataRequest request;
    request.setCurrentPart(0);
    request.setPackageId(header->packageId());

    return thiz->sendData(&request, sender, &hdr);
}

bool AbstractNodeParser::processPart(const QSharedPointer<PKG::BigDataPart> &part,
                                 const AbstractNodeInfo *sender,
                                 const Header & hdr) {

    if (!_pool.contains(part->packageId())) {
        return false;
    }

    checkOutDatedPacakges(part->packageId());

    auto& localPool = _pool[part->packageId()];

    localPool.chaindata[part->getPakckageNumber()] = part;

    auto sendRequest = [sender, &hdr, this](int part, unsigned int id) {

        PKG::BigDataRequest request;
        request.setCurrentPart(part);
        request.setPackageId(id);

        return thiz->sendData(&request, sender, &hdr);
    };

    if (part->getPakckageNumber() + 1 < localPool.chaindata.size()) {
        return sendRequest(part->getPakckageNumber() + 1,
                           part->packageId());

    }

    auto package = genPackage(localPool.header->getCommand());
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

    if (parsePackage(package, hdr, sender) == ParserResult::Error) {
        return false;
    }

    _pool.remove(part->packageId());

    return true;
}

bool AbstractNodeParser::processRequest(const QSharedPointer<PKG::BigDataRequest> &request,
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
    if (!thiz->sendData(data.data(), sender, &pkgHeader)) {
        return false;
    }

    if (fLast) {
        _pool.remove(id);
    }

    return true;
}

bool AbstractNodeParser::sendBigDataPackage(const PKG::AbstractData *data,
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


    if (!thiz->sendData(hdr.data(), sender, pkgHeader)) {
        return false;
    }

    return true;
}

void AbstractNodeParser::checkOutDatedPacakges(unsigned int currentProcessedId) {
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
