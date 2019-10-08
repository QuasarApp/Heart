#include "item.h"
#include "factorynetobjects.h"
#include <exception>
#include <quasarapp.h>

int Item::getId() const {
    return id;
}

bool Item::setId(int value) {
    id = value;

    if (data.size() < static_cast<int>(sizeof (id) + sizeof (ClientProtocol::Command))) {
        return false;
    }

    int oldSize = data.size();

    data.replace(sizeof (ClientProtocol::Command), sizeof (id),
                 reinterpret_cast<char*>(&id), sizeof (id));

    return data.size() == oldSize;
}

Item::Item() {

}

Item::Item(const ClientProtocol::Package &other) {
    hdr = other.hdr;
    data = other.data;
    ClientProtocol::BaseNetworkObject base;
    base.fromBytes(data);
    id = base.id();
}

Item::Item(const ClientProtocol::BaseNetworkObject *obj) {
    if (!create(obj, ClientProtocol::Type::Stream)) {
        QuasarAppUtils::Params::verboseLog("Error create Item from BaseNetworkObject",
                                           QuasarAppUtils::VerboseLvl::Error);
    }

    id = obj->id();
}

Item::Item(ClientProtocol::Command cmd, const QByteArray &data) {
    if (!create(cmd, ClientProtocol::Type::Stream, data)) {
        QuasarAppUtils::Params::verboseLog("Error create Item from QByteArray",
                                           QuasarAppUtils::VerboseLvl::Error);
    }

    ClientProtocol::BaseNetworkObject base;
    base.fromBytes(data);
    id = base.id();
}

Item::~Item() {

}

ClientProtocol::Command Item::cmd() const {
    return static_cast<ClientProtocol::Command>(hdr.command);
}

const QByteArray &Item::dataArray() const {
    return data;
}

bool Item::isValid() const {
    return ClientProtocol::FactoryNetObjects::isRegisteredType(hdr.type)
            && Package::isValid();
}
