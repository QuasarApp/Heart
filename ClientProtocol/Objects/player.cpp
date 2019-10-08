#include "player.h"
#include "config.h"
#include <clientprotocol.h>

namespace ClientProtocol {


QString Player::getName() const {
    return name;
}

void Player::setName(const QString &value) {
    name = value;
}

QString Player::getGmail() const {
    return gmail;
}

void Player::setGmail(const QString &value) {
    gmail = value;
}

unsigned int Player::getMany() const
{
    return money;
}

void Player::setMany(unsigned int value) {
    money = value;
}

unsigned int Player::getRecord() const {
    return record;
}

void Player::setRecord(unsigned int value) {
    record = value;
}

QList<int> Player::getItems() const {
    return items;
}

void Player::setItems(const QList<int> &value) {
    items = value;
}

int Player::getCureentSnake() const {
    return cureentSnake;
}

void Player::setCureentSnake(int value) {
    cureentSnake = value;
}


unsigned int Player::getAvgRecord() const {
    return avgRecord;
}

void Player::setAvgRecord(unsigned int value) {
    avgRecord = value;
}

Player::Player() {
    _class = static_cast<quint8>(Command::Player);
    
}

BaseNetworkObject *Player::create() const {
    return new Player();
}

NetworkClassSize Player::classSize() const {

    return UpdatePlayerData::classSize()
            + getTypeSize(name)
            + getTypeSize(gmail)
            + getTypeSize(money)
            + getTypeSize(record)
            + getTypeSize(avgRecord)
            + getTypeSize(items)
            + getTypeSize(cureentSnake);
}

QDataStream &Player::writeToStream(QDataStream &stream) const {
    UpdatePlayerData::writeToStream(stream);
    stream << name;
    stream << gmail;
    stream << money;
    stream << record;
    stream << avgRecord;
    stream << items;
    stream << cureentSnake;

    return stream;
}

QDataStream &Player::readFromStream(QDataStream &stream) {
    UpdatePlayerData::readFromStream(stream);
    stream >> name;
    stream >> gmail;
    stream >> money;
    stream >> record;
    stream >> avgRecord;
    stream >> items;
    stream >> cureentSnake;
    return stream;
}

bool Player::isValid() const {
    return !name.isNull() && gmail.size() > 5 &&
            BaseNetworkObject::isValid();
}
}
