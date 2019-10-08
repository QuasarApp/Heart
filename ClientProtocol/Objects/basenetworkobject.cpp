#include "basenetworkobject.h"

namespace ClientProtocol {



int BaseNetworkObject::id() const {
    return _id;
}

void BaseNetworkObject::setId(int id) {
    _id = id;
}

quint8 BaseNetworkObject::getClass() const {
    return _class;
}

BaseNetworkObject::BaseNetworkObject() {
}

BaseNetworkObject *BaseNetworkObject::create() const {
    return new BaseNetworkObject();
}

BaseNetworkObject::~BaseNetworkObject() {

}

NetworkClassSize BaseNetworkObject::classSize() const {
    return getTypeSize(_id) + getTypeSize(_class);
}

QDataStream &BaseNetworkObject::writeToStream(QDataStream &stream) const {
    stream << _class;
    stream << _id;
    return stream;
}

QDataStream &BaseNetworkObject::readFromStream(QDataStream &stream) {
    stream >> _class;
    stream >> _id;
    return stream;
}

bool BaseNetworkObject::isValid() const {
    return _id >= 0 && _class > 0;
}

void BaseNetworkObject::toBytes(QByteArray &array) const {
    QDataStream stream(&array, QIODevice::WriteOnly);

    writeToStream(stream);

}

void BaseNetworkObject::fromBytes(const QByteArray &array) {
    QDataStream stream(array);

    readFromStream(stream);
}

NetworkClassSize getTypeSize(const EncryptionParams &params) {
    bool isKey = params.alg & Key;

    switch (params.alg & ~Key) {
    case RSA: {

        if (isKey) {

            return {
                static_cast<unsigned int>(sizeof (int) +
                                          QRSAEncryption::getKeyBytesSize(static_cast<QRSAEncryption::Rsa>(params.baseBits)))
            };
        }

        auto baseSize =
                static_cast<unsigned int>(sizeof (int) +
                                          QRSAEncryption::getKeyBytesSize(static_cast<QRSAEncryption::Rsa>(params.encryptBits))) / 2;

        return {baseSize, baseSize * params.baseBytes()};
    }

    case SHA: {
        return {static_cast<unsigned int>(sizeof (int) + params.baseBytes())};
    }
    default:
        return sizeof (int) + 10;
    }


}

unsigned int EncryptionParams::baseBytes() const {
    return baseBits / 8;
}

}
