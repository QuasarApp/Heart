#include "databasenode.h"
#include "databasenodeparser.h"

#include <deleteobject.h>
#include <websocket.h>
#include <websocketsubscriptions.h>
#include <badrequest.h>

namespace QH {

#define thiz node<DataBaseNode>()

DataBaseNodeParser::DataBaseNodeParser(DataBaseNode* node):
    AbstractNodeParser(node) {


    registerPackageType<PKG::WebSocket>();
    registerPackageType<PKG::WebSocketSubscriptions>();
    registerPackageType<PKG::DeleteObject>();
}

ParserResult DataBaseNodeParser::parsePackage(const QSharedPointer<PKG::AbstractData> &pkg,
                                              const Header &pkgHeader,
                                              const AbstractNodeInfo *sender) {

    auto parentResult = AbstractNodeParser::parsePackage(pkg, pkgHeader, sender);
    if (parentResult != ParserResult::NotProcessed) {
        return parentResult;
    }

    if (!thiz)
        return ParserResult::Error;

    if (PKG::WebSocket::command() == pkg->cmd()) {
        PKG::WebSocket *obj = static_cast<PKG::WebSocket*>(pkg.data());

        QVariant requesterId = thiz->getSender(sender, obj);
        if (!obj->isValid()) {
            thiz->badRequest(sender->networkAddress(), pkgHeader,
                       {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        if (!workWithSubscribe(*obj, requesterId, sender)) {
            thiz->badRequest(sender->networkAddress(), pkgHeader, {
                           ErrorCodes::InvalidRequest,
                           "WebSocket request is invalid"
                       });
            return ParserResult::Error;
        }

        return ParserResult::Processed;

    }

    return ParserResult::NotProcessed;

}

bool DataBaseNodeParser::workWithSubscribe(const PKG::WebSocket &rec,
                                     const QVariant &clientOrNodeid,
                                     const AbstractNodeInfo * sender) {

    auto _db = db();
    if (!_db)
        return false;

    switch (static_cast<PKG::WebSocketRequest>(rec.getRequestCmd())) {

    case PKG::WebSocketRequest::Subscribe: {
        _webSocketWorker->subscribe(clientOrNodeid, rec.subscribeId());
        memberSubsribed(clientOrNodeid, rec.subscribeId());
        return true;
    }

    case PKG::WebSocketRequest::Unsubscribe: {
        _webSocketWorker->unsubscribe(clientOrNodeid, rec.subscribeId());
        memberUnsubsribed(clientOrNodeid, rec.subscribeId());
        return true;
    }

    case PKG::WebSocketRequest::SubscribeList: {

        WebSocketSubscriptions resp;
        resp.setAddresses(_webSocketWorker->list(clientOrNodeid));

        return sendData(&resp, sender);
    }

    default: break;
    }

    return false;
}


}
