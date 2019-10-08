#include "snake.h"
#include "clientprotocol.h"


namespace ClientProtocol {

quint8 Snake::getSpeed() const {
    return speed;
}

void Snake::setSpeed(const quint8 &value) {
    speed = value;
}

quint8 Snake::getSnakeClass() const {
    return snakeClass;
}

void Snake::setSnakeClass(const quint8 &value) {
    snakeClass = value;
}

QList<float> Snake::getSkillet() const {
    return skillet;
}

void Snake::setSkillet(const QList<float> &value) {
    skillet = value;
}

BaseNetworkObject *Snake::create() const {
    return new Snake();
}

NetworkClassSize Snake::classSize() const {
    return BaseNetworkObject::classSize() +
            getTypeSize(speed) +
            getTypeSize(snakeClass) +
            getTypeSize(skillet);
}

QDataStream &Snake::writeToStream(QDataStream &stream) const {
    BaseNetworkObject::writeToStream(stream);
    stream << speed;
    stream << snakeClass;
    stream << skillet;

    return stream;
}

QDataStream &Snake::readFromStream(QDataStream &stream) {
    BaseNetworkObject::readFromStream(stream);
    stream >> speed;
    stream >> snakeClass;
    stream >> skillet;
    return stream;
}

bool Snake::isValid() const {
    return (speed > 0) && skillet.size() && BaseNetworkObject::isValid();
}

Snake::Snake() {
    _class = static_cast<quint8>(Command::Snake);

}

}
