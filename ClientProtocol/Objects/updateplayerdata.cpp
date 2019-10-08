#include "updateplayerdata.h"
#include "config.h"
#include "clientprotocol.h"

namespace ClientProtocol {

UpdatePlayerData::UpdatePlayerData() {
    _class = static_cast<quint8>(Command::UpdatePlayerData);

}

QByteArray UpdatePlayerData::getToken() const {
    return token;
}

void UpdatePlayerData::setToken(const QByteArray &value) {
    token = value;
}

BaseNetworkObject *UpdatePlayerData::create() const {
    return new UpdatePlayerData();
}

NetworkClassSize UpdatePlayerData::classSize() const {
    EncryptionParams param = {
        cryptoAlghoritms::SHA,
        BASE_HASH_BITS
    };

    return BaseNetworkObject::classSize() + getTypeSize(param);
}

QDataStream &UpdatePlayerData::writeToStream(QDataStream &stream) const {
    BaseNetworkObject::writeToStream(stream);
    stream << token;
    return stream;
}

QDataStream &UpdatePlayerData::readFromStream(QDataStream &stream) {
    BaseNetworkObject::readFromStream(stream);
    stream >> token;
    return stream;
}

bool UpdatePlayerData::isValid() const {
    return token.size() == 32 && BaseNetworkObject::isValid();
}

}
