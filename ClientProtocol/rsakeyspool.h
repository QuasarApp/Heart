#ifndef RSAKEYSPOOL_H
#define RSAKEYSPOOL_H
#include <QHash>
#include <QObject>
#include <qrsaencryption.h>
#include <QMutex>
#include <QMap>
#include "clientprotocol_global.h"

namespace ClientProtocol {

struct RSAKeyPair {
    QByteArray pub;
    QByteArray priv;
};

typedef QMap<int, RSAKeyPair> KeysPool;
typedef QHash<QRSAEncryption::Rsa, KeysPool> PoolData;
class CLIENTPROTOCOLSHARED_EXPORT RSAKeysPool: public QObject {
    Q_OBJECT
private:
    PoolData pool;

    QMutex multithread;

public:
    bool take(QRSAEncryption::Rsa rsa, RSAKeyPair &res);
    void addNewKey(QRSAEncryption::Rsa rsa, const RSAKeyPair& key);
    int size(QRSAEncryption::Rsa rsa) const;

signals:

    void sigKeyTaked();
};

}
#endif // RSAKEYSPOOL_H
