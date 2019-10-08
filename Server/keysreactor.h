#ifndef KEYSREACTOR_H
#define KEYSREACTOR_H

#include <qrsaencryption.h>
#include <QtConcurrent>
#include <rsakeyspool.h>
#include "server_global.h"

#define DEFAULT_KEYPOOL_SIZE 10

class SERVERSHARED_EXPORT KeysReactor: public QObject
{
    Q_OBJECT
private:
    ClientProtocol::RSAKeysPool _pool;
    int _poolSize = DEFAULT_KEYPOOL_SIZE;
    QHash<QRSAEncryption::Rsa, bool> _mutexs;
    QHash<QRSAEncryption::Rsa, QFuture<void>> _futures;
    void generateKeys(QRSAEncryption::Rsa);

private slots:
    void handleGenerateNewKeys();

public:
    KeysReactor(bool ForceGenerateKey, QObject *ptr = nullptr);

    ~KeysReactor() override;

    ClientProtocol::RSAKeysPool* getPool();
    int getPoolSize() const;
    void setPoolSize(int value);

};

#endif // KEYSREACTOR_H
