#ifndef PUBKEY_H
#define PUBKEY_H
#include <qrsaencryption.h>

#include "basenetworkobject.h"

namespace ClientProtocol {

class CLIENTPROTOCOLSHARED_EXPORT PubKey: public BaseNetworkObject
{
private:
    QRSAEncryption::Rsa typeKey;
    QByteArray key;

public:
    PubKey();

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;

    QRSAEncryption::Rsa getTypeKey() const;
    void setTypeKey(const QRSAEncryption::Rsa &value);
    QByteArray getKey() const;
    void setKey(const QByteArray &value);
};
}
#endif // PUBKEY_H
