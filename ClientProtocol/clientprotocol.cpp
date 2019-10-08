#include "clientprotocol.h"
#include "gamedata.h"
#include "getitem.h"
#include "login.h"
#include "player.h"
#include "updateplayerdata.h"

#include <QDataStream>
#include <QVariantMap>
#include <factorynetobjects.h>
#include <map.h>
#include <pubkey.h>
#include <snake.h>
#include <websocket.h>

#define DEFAULT_GAME_PORT 7777

namespace ClientProtocol {


Header::Header() {
    reset();
}

bool Header::isValid() const {

    if (sizeof (*this) != 4) {
        return false;
    }

    if (static_cast<Command>(command) == Command::Undefined) {
        return false;
    }

    if (static_cast<Type>(type) == Type::Undefined) {
        return false;
    }

    if (static_cast<Type>(type) == Type::Responke) {
        return static_cast<Command>(command) != Command::Undefined;
    }

    return isValidSize(command, size);
}

void Header::reset() {
    size = 0;
    command = static_cast<quint8>(Command::Undefined);
    type = static_cast<quint8>(Type::Responke);
}

Package::Package() {
    reset();
}

bool Package::isValid() const {
    if (!hdr.isValid()) {
        return false;
    }

    if (data.size() && hdr.command != data.at(0)) {
        return false;
    }

    return hdr.size == static_cast<unsigned int> (data.size());
}

BaseNetworkObject* Package::parse() const {
    if (!isValid())
        return nullptr;

    auto obj = FactoryNetObjects::build(hdr.command);

    if (!obj) {
        return nullptr;
    }

    QDataStream stream(data);
    obj->readFromStream(stream);

    return obj;
}


bool Package::create(const BaseNetworkObject *obj, Type type, const Header *old) {

    if (!obj) {
        return false;
    }

    auto command = static_cast<Command>(obj->getClass());

    if (command == Command::Undefined) {
        return false;
    }

    QDataStream stream(&data, QIODevice::ReadWrite);
    obj->writeToStream(stream);

    return create(command, type, old);
}

bool Package::create(Command cmd, Type type, const QByteArray &data, const Header* old) {
    this->data = data;
    return create(cmd, type, old);
}

bool Package::create(Command cmd, Type type, const Header *old) {


    if (cmd == Command::Undefined) {
        return false;
    }

    hdr.command = static_cast<quint8>(cmd);
    hdr.type = static_cast<quint8>(type);
    hdr.size = static_cast<unsigned short>(data.size());

    if (type == Type::Responke) {
        signPackage(old);
    }

    return isValid();
}

QByteArray Package::toBytes() const {
    QByteArray res;
    res.append(reinterpret_cast<char*>(const_cast<Header*>(&hdr)),
               sizeof (hdr));

    res.append(data);
    return res;
}

void Package::reset() {
    hdr.reset();
    data.clear();
}

bool Package::signPackage(const Header *oldHeader) {

    if (!oldHeader || !oldHeader->isValid()) {
        return false;
    }

    hdr.sig = oldHeader->sig;
    hdr.requestCommand = oldHeader->command;

    return true;
}

bool isValidSize(quint8 type, unsigned int size) {

    if (!FactoryNetObjects::isInited()) {
        return false;
    }

    if (!FactoryNetObjects::isRegisteredType(type)) {
        return size == 0;
    }

    return FactoryNetObjects::getSize(type).isValid(size);
}

bool initClientProtockol() {
    if (!FactoryNetObjects::regType<Login>(
                static_cast<quint8>(Command::Login))) {
        return false;
    }

    if (!FactoryNetObjects::regType<UpdatePlayerData>(
                static_cast<quint8>(Command::UpdatePlayerData))) {
        return false;
    }

    if (!FactoryNetObjects::regType<GameData>(
                static_cast<quint8>(Command::GameData))) {
        return false;
    }

    if (!FactoryNetObjects::regType<GetItem>(
                static_cast<quint8>(Command::GetItem))) {
        return false;
    }

    if (!FactoryNetObjects::regType<Player>(
                static_cast<quint8>(Command::Player))) {
        return false;
    }

    if (!FactoryNetObjects::regType<Snake>(
                static_cast<quint8>(Command::Snake))) {
        return false;
    }

    if (!FactoryNetObjects::regType<Map>(
                static_cast<quint8>(Command::Map))) {
        return false;
    }

    if (!FactoryNetObjects::regType<PubKey>(
                static_cast<quint8>(Command::PubKey))) {
        return false;
    }

    if (!FactoryNetObjects::regType<WebSocket>(
                static_cast<quint8>(Command::WebSocket))) {
        return false;
    }

    return true;
}

}
